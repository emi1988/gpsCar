#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifndef DEFS_H
#include "defs.h"
#endif

#ifndef DBMANAGER_H
#include "dbManager.h"
#endif

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QNetworkReply>
#include <QDateTime>

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
    quint64 m_lastSendetTime;
    int m_secondsBetweenWebSend;
    dbManager *m_dbManager;

    stSerialPortSettings m_currentPortSettings;

    bool getSerialPortSettings();
    bool openSerialPort();
    void convertToDecimalCoordinates(QString nmeaData, QString alignment, QString &decimalData);

    void sendDataToServer(stGPSdata gpsDataToSend, bool bufferData);


private slots:
    void handleError(QSerialPort::SerialPortError error);
    void serialDataReceived();
    void networkReplyReceived();
    void networkReplyError(QNetworkReply::NetworkError error);
    void resendOldData();
    void updateLogUi(const QString &text);
};

#endif // MAINWINDOW_H
