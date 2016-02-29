#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QNetworkAccessManager>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //init the database for buffering the gps data if there's no network
    QString dbPath = QDir::currentPath() + "/sendBuffer.db";
    m_dbManager = dbManager(dbPath);

    m_secondsBetweenWebSend = 5;
    m_lastSendetTime = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000;

    m_serialPort = new QSerialPort(this);

    m_networkManager = new QNetworkAccessManager;
    m_serverAdress = "http://web568.lenny.servertools24.de/raspiGPS/raspiCarGpsReceive.php";

    if(getSerialPortSettings() == true)
    {
        //gps receiver found, so open serial port
        openSerialPort();

    }

    connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));

     connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(serialDataReceived()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::getSerialPortSettings()
{
//go through all aviable serial ports
    bool foundGPSreceiver = false;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        //the number 8963 (in hex) is the ID from my GPS-receiver
        if(info.productIdentifier() == 8963 )
        {
            m_currentPortSettings.portName = info.portName();
            foundGPSreceiver = true;
            qDebug() <<"GPS-receiver found on port:" << info.portName();
            qDebug() << "description: " << info.description();
            qDebug() << "manufacturer: " << info.manufacturer();

            break;
        }
    }

if(foundGPSreceiver == true)
{
    //set all other settings

    m_currentPortSettings.baudRate = QSerialPort::Baud4800;
    m_currentPortSettings.dataBits = QSerialPort::Data8;
    m_currentPortSettings.flowControl = QSerialPort::NoFlowControl;
    m_currentPortSettings.parity = QSerialPort::NoParity;
    m_currentPortSettings.stopBits = QSerialPort::OneStop;

}
else
{
    qDebug() << "GPS-receiver not found ): ";
}

    return foundGPSreceiver;

}

bool MainWindow::openSerialPort()
{
    //first set the current settings
    m_serialPort->setPortName(m_currentPortSettings.portName);
    m_serialPort->setBaudRate(m_currentPortSettings.baudRate);
    m_serialPort->setDataBits(m_currentPortSettings.dataBits);
    m_serialPort->setFlowControl(m_currentPortSettings.flowControl);
    m_serialPort->setParity(m_currentPortSettings.parity);
    m_serialPort->setStopBits(m_currentPortSettings.stopBits);


     if (m_serialPort->open(QIODevice::ReadWrite))
     {
         qDebug()<<"serial port successfully opened";

         return true;
     }
     else
     {
         qDebug()<<"could not open serial port ): ";

         return false;
     }

}

void MainWindow::convertToDecimalCoordinates(QString nmeaData, QString alignment, QString &decimalData)
{

    QStringList splitedData =  nmeaData.split(".");

    //4 characters = latitude
    if(splitedData.at(0).size() == 4)
    {
        // nmeaData = XXYY.ZZZZ
        //conversion XXYY.ZZZZ ⇒ XX° + (YY.ZZZZ / 60)°

        int xx = splitedData.at(0).left(2).toInt();
        int yy = splitedData.at(0).right(2).toInt();
        int zzzz = splitedData.at(1).toInt();

        double decimalDataDouble = xx + ( (yy + zzzz*0.0001) / 60 );

        //values under the equator are negativ
        if(alignment.compare("S") == 0)
        {
            decimalDataDouble = decimalDataDouble * (-1.0);
        }

        decimalData =QString::number(decimalDataDouble);

    }
    //5 characters = longitudenetworkReplyError
    else
    {
        // nmeaData =XXXYY.ZZZZ
        //conversion XXXYY.ZZZZ ⇒ XXX° + (YY.ZZZZ / 60)°

        int xxx = splitedData.at(0).left(3).toInt();
        int yy = splitedData.at(0).right(2).toInt();
        int zzzz = splitedData.at(1).toInt();

        double decimalDataDouble = xxx + ( (yy + zzzz*0.0001) / 60 );

        if(alignment.compare("W") == 0)
        {
            decimalDataDouble = decimalDataDouble * (-1.0);
        }
        decimalData =QString::number(decimalDataDouble);

    }
}

void MainWindow::sendDataToServer()
{

    //generate the post-data
    QString dataToSend;

    dataToSend.append("timeStampGPS=" + m_currentGPSdata.timeStampGPS);
    dataToSend.append("&timeStampRapi=" + m_currentGPSdata.timeStampRapi);
    dataToSend.append("&longitudeDecimal=" + m_currentGPSdata.longitudeDecimal);
    dataToSend.append("&latitudeDecimal=" + m_currentGPSdata.latitudeDecimal);
    dataToSend.append("&altitude=" + m_currentGPSdata.altitude);
    dataToSend.append("&satelliteAmount=" + m_currentGPSdata.satelliteAmount);
    dataToSend.append("&horizontalPrecision=" + m_currentGPSdata.horizontalPrecision);

    //fist save the data into the buffer-database
    m_dbManager.addGpsData(dataToSend);

    QByteArray dataByteArray = dataToSend.toLatin1();

    QNetworkRequest request;
    request.setUrl(QUrl(m_serverAdress));
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    m_reply = m_networkManager->post(request,dataByteArray);

    connect(m_reply, SIGNAL(readyRead()), this, SLOT(networkReplyReceived()));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(networkReplyError(QNetworkReply::NetworkError)));

}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{

    qDebug() << "QSerialPort Error Number" << error;

}

void MainWindow::serialDataReceived()
{

    QString currentReceivedData = QString(m_serialPort->readAll());

    //add the buffered data from the last receive run
    currentReceivedData.prepend(m_receiveBuffer);

    //search for the start-character "$"
    QStringList currentGPSsentences = currentReceivedData.split("$");
    //save the last incomplete part in the buffer
    m_receiveBuffer.clear();
    m_receiveBuffer.append(currentGPSsentences.last());
    //remove the last incomplete sentence from the buffer
    currentGPSsentences.removeLast();

    //go through all sentences

    foreach (QString currentSentence, currentGPSsentences)
    {
        QStringList singleSentenceData = currentSentence.split(",");

        //just use the GPGGA-Information and check if it's already received completely
        if((singleSentenceData.at(0).compare("GPGGA")== 0) & (singleSentenceData.count() == 15))
        {

            //just save/send the gps-data if the m_msecondsBetweenWebSend is over
            quint64 currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000;

            if(currentTime > (m_lastSendetTime + m_secondsBetweenWebSend) )
            {
                m_lastSendetTime = currentTime;

                QStringList singleSentenceData = currentSentence.split(",");
                qDebug() << "GPGGA-Data: " << singleSentenceData;
                qDebug() << "RapiTimestamp: " << currentTime;
                //GPGGA-Data:  ("GPGGA", "213225.936", "", "", "", "", "0", "00", "", "", "M", "0.0", "M", "", "0000*5F ")
                //$GPGGA,191410,4735.5634,N,00739.3538,E,1,04,4.4,351.5,M,48.0,M,,*45    //sum =15

                //save the current GPS-data
                m_currentGPSdata.timeStampGPS = singleSentenceData.at(1);
                m_currentGPSdata.latitude = singleSentenceData.at(2);
                m_currentGPSdata.latitudeAlignment = singleSentenceData.at(3);
                m_currentGPSdata.longitude = singleSentenceData.at(4);
                m_currentGPSdata.longitudeAlignment = singleSentenceData.at(5);
                m_currentGPSdata.satelliteAmount= singleSentenceData.at(7);
                m_currentGPSdata.horizontalPrecision = singleSentenceData.at(8);
                m_currentGPSdata.altitude = singleSentenceData.at(9);

                m_currentGPSdata.timeStampRapi = QString::number(currentTime);

                //convert position to decimal-degree
                convertToDecimalCoordinates(m_currentGPSdata.latitude, m_currentGPSdata.latitudeAlignment, m_currentGPSdata.latitudeDecimal);
                convertToDecimalCoordinates(m_currentGPSdata.longitude, m_currentGPSdata.longitudeAlignment, m_currentGPSdata.longitudeDecimal);

                //send data to server
                sendDataToServer();

                qDebug() << "test";
            }
        }
    }

    //qDebug() << currentGPSsentences;


}

void MainWindow::networkReplyReceived()
{
    QString replyString = QString(m_reply->readAll());
    qDebug() << "network reply" << replyString;

    QStringList splitedString = replyString.split(":");

    QString rapiTimestamp = splitedString.at(1);

    m_dbManager.removeGpsData(rapiTimestamp);


}

void MainWindow::networkReplyError(QNetworkReply::NetworkError error)
{
    qDebug() <<"networkReplyError received: " << error ;
}

