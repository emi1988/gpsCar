#ifndef DBMANAGER_H
#define DBMANAGER_H
#include <QSqlDatabase>

class dbManager
{
public:
    dbManager();
    dbManager(QString& path);

    bool addGpsData(QString data);
    bool removeGpsData(QString timeStampRapi);

private:
    QSqlDatabase m_sqliteDb;
};

#endif // DBMANAGER_H
