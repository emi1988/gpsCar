#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);

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

        }
    }

    //qDebug() << currentGPSsentences;


}
