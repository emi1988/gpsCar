#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QNetworkAccessManager>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);

    *networkManager = new QNetworkAccessManager;
    serverAdress = "http://web568.lenny.servertools24.de/raspiGPS/raspiCarGpsReceive.php";

    if(getSerialPortSettings() == true)
    {
        //gps receiver found, so open serial port
        openSerialPort();

    }

    connect(serial, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(handleError(QSerialPort::SerialPortError)));

     connect(serial, SIGNAL(readyRead()), this, SLOT(serialDataReceived()));

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
            currentPortSettings.portName = info.portName();
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

    currentPortSettings.baudRate = QSerialPort::Baud4800;
    currentPortSettings.dataBits = QSerialPort::Data8;
    currentPortSettings.flowControl = QSerialPort::NoFlowControl;
    currentPortSettings.parity = QSerialPort::NoParity;
    currentPortSettings.stopBits = QSerialPort::OneStop;

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
    serial->setPortName(currentPortSettings.portName);
    serial->setBaudRate(currentPortSettings.baudRate);
    serial->setDataBits(currentPortSettings.dataBits);
    serial->setFlowControl(currentPortSettings.flowControl);
    serial->setParity(currentPortSettings.parity);
    serial->setStopBits(currentPortSettings.stopBits);


     if (serial->open(QIODevice::ReadWrite))
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
    //5 characters = longitude
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

bool MainWindow::sendDataToServer()
{
    //generate the post-data





}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{

    qDebug() << "QSerialPort Error Number" << error;

}

void MainWindow::serialDataReceived()
{

    QString currentReceivedData = QString(serial->readAll());

    //add the buffered data from the last receive run
    currentReceivedData.prepend(receiveBuffer);

    //search for the start-character "$"
    QStringList currentGPSsentences = currentReceivedData.split("$");
    //save the last incomplete part in the buffer
    receiveBuffer.clear();
    receiveBuffer.append(currentGPSsentences.last());
    //remove the last incomplete sentence from the buffer
    currentGPSsentences.removeLast();

    //go through all sentences

    foreach (QString currentSentence, currentGPSsentences)
    {
        if(currentSentence.startsWith("GPGGA"))
        {
            QStringList singleSentenceData = currentSentence.split(",");
            qDebug() << "GPGGA-Data: " << singleSentenceData;
            //GPGGA-Data:  ("GPGGA", "213225.936", "", "", "", "", "0", "00", "", "", "M", "0.0", "M", "", "0000*5F ")
            //$GPGGA,191410,4735.5634,N,00739.3538,E,1,04,4.4,351.5,M,48.0,M,,*45

            //save the current GPS-data
            currentGPSdata.timeStamp = singleSentenceData.at(1);
            currentGPSdata.latitude = singleSentenceData.at(2);
            currentGPSdata.latitudeAlignment = singleSentenceData.at(3);
            currentGPSdata.longitude = singleSentenceData.at(4);
            currentGPSdata.longitudeAlignment = singleSentenceData.at(5);
            currentGPSdata.satelliteAmount= singleSentenceData.at(7);
            currentGPSdata.horizontalPrecision = singleSentenceData.at(8);
            currentGPSdata.altitude = singleSentenceData.at(9);

            //convert position to decimal-degree
            convertToDecimalCoordinates(currentGPSdata.latitude, currentGPSdata.latitudeAlignment, currentGPSdata.latitudeDecimal);
            convertToDecimalCoordinates(currentGPSdata.longitude, currentGPSdata.longitudeAlignment, currentGPSdata.longitudeDecimal);

            qDebug() << "test";

        }
    }

    //qDebug() << currentGPSsentences;


}
