#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QSqlDatabase>


#ifndef DEFS_H
#include "defs.h"
#endif

class dbManager: public QObject
{
    Q_OBJECT
public:
    dbManager();

    bool addGpsData(QString data);
    bool removeGpsData(QString timeStampRapi);
    bool getOldGpsData(QString maxTimestampRapi, QList<stGPSdata> &selectedData);

    void initDb(QString &path);
signals:
    void  sendText(const QString &text);

private:
    QSqlDatabase m_sqliteDb;


};

#endif // DBMANAGER_H
