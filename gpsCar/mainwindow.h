#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef DEFS_H
#include "defs.h"
#endif

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSerialPort *serial;
    QString receiveBuffer;
    stGPSdata currentGPSdata;

     QNetworkAccessManager *networkManager;
     QString serverAdress;

    stSerialPortSettings currentPortSettings;

    bool getSerialPortSettings();
    bool openSerialPort();
    void convertToDecimalCoordinates(QString nmeaData, QString alignment, QString &decimalData);

    bool sendDataToServer();


private slots:
    void handleError(QSerialPort::SerialPortError error);
     void serialDataReceived();
};

#endif // MAINWINDOW_H
