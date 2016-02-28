#ifndef DEFS_H
#define DEFS_H

#include <QString>
#include <QtSerialPort/QSerialPort>

typedef struct stSerialPortSettings
{
    QString portName;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    QSerialPort::FlowControl flowControl;
};

typedef struct stGPSdata
{
    QString timeStampGPS;
    QString longitude; //längengrad
    QString longitudeAlignment; //E(ast) or W(est)
    QString latitude; //breitengrad
    QString latitudeAlignment; // N(orth) or S(outh)
    QString satelliteAmount;
    QString horizontalPrecision;
    QString altitude;

    QString longitudeDecimal;
    QString latitudeDecimal;
    QString timeStampRapi;
};


#endif // DEFS_H
