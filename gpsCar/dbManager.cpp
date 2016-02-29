#include "dbManager.h"
#include <QDebug>
#include <QSqlQuery>

dbManager::dbManager()
{

}

dbManager::dbManager(QString &path)
{
    m_sqliteDb = QSqlDatabase::addDatabase("QSQLITE");
    m_sqliteDb.setDatabaseName(path);

    if (!m_sqliteDb.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

bool dbManager::addGpsData(QString data)
{

    QStringList keyValuePairs = data.split("&");

    QString keys = "(";
    QString values = "(";

    for (int i = 0; i < keyValuePairs.count(); ++i)
    {

        QStringList currentKeyValue =  keyValuePairs.at(i).split("=");
        keys.append(currentKeyValue.at(0) + ",");
        values.append("'" + currentKeyValue.at(1)+ "'" + ",");
    }

    keys.remove(keys.length()-1,1);
    keys.append(")");
    values.remove(values.length()-1,1);
    values.append(")");

    QString sqlCommand = "INSERT INTO sendBuffer ";
    sqlCommand.append(keys);
    sqlCommand.append( " VALUES " + values);

    bool success = false;

    QSqlQuery query;
    query.prepare(sqlCommand);

    if(query.exec())
    {
        success = true;
        qDebug() << "values saved in database sendbuffer";
    }
    else
    {
        success = false;
        qDebug() << "could NOT save values saved in database sendbuffer";
    }

    return success;
}
