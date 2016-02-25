#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef DEFS_H
#include "defs.h"
#endif

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
 #include <QNetworkReply>

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

    QSerialPort *m_serialPort;
    QString m_receiveBuffer;
    stGPSdata m_currentGPSdata;

     QNetworkAccessManager *m_networkManager;
     QNetworkReply *m_reply;
     QString m_serverAdress;

    stSerialPortSettings m_currentPortSettings;

    bool getSerialPortSettings();
    bool openSerialPort();
    void convertToDecimalCoordinates(QString nmeaData, QString alignment, QString &decimalData);

    void sendDataToServer();


private slots:
    void handleError(QSerialPort::SerialPortError error);
    void serialDataReceived();
    void networkReplyReceived();
    void networkReplyError(QNetworkReply::NetworkError error);
};

#endif // MAINWINDOW_H
