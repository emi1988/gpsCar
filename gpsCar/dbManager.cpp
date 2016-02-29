#include "dbManager.h"
#include <QDebug>
#include <QSqlQuery>
#include <QFile>

dbManager::dbManager()
{

}

dbManager::dbManager(QString &path)
{
    //first check if database already exists
    bool dbExists = QFile::exists(path);

    //if db doesn't exist it's automatically created
    m_sqliteDb = QSqlDatabase::addDatabase("QSQLITE");
    m_sqliteDb.setDatabaseName(path);

    if (!m_sqliteDb.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";

        //if the database was just created above, we have to create a table
        if(dbExists == false)
        {
            QString createStatement = " CREATE TABLE \"sendBuffer\" ";
            createStatement.append("(`ID` INTEGER PRIMARY KEY AUTOINCREMENT,");
            createStatement.append(" `timeStampGPS` TEXT, `timeStampRapi` TEXT, `longitudeDecimal` TEXT, `latitudeDecimal` TEXT, `altitude` TEXT, `satelliteAmount` TEXT, `horizontalPrecision` TEXT )");
            QSqlQuery query(m_sqliteDb);

            query.prepare(createStatement);
            if(query.exec())
            {
                qDebug() << "new database and table created";
            }
            else
            {
                qDebug() << "error: could not create new database and table";
            }
        }
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

    QSqlQuery query(m_sqliteDb);
    //bool test = query.exec(sqlCommand);

    query.prepare(sqlCommand);


    if(query.exec())
    {
        success = true;
        qDebug() << "values saved in database sendbuffer \n";
    }
    else
    {
        success = false;
        qDebug() << "could NOT save values in database sendbuffer \n";
    }

    return success;
}

bool dbManager::removeGpsData(QString timeStampRapi)
{
    QString sqlCommand = "DELETE FROM `sendBuffer` WHERE `timeStampRapi`='";
    sqlCommand .append(timeStampRapi + "'");

    QSqlQuery query(m_sqliteDb);

    query.prepare(sqlCommand);

    bool success;
    if(query.exec())
    {
        success = true;
        qDebug() << "value removed from buffer \n";
    }
    else
    {
        success = false;
        qDebug() << "could NOT remove value from buffer \n";
    }

    return success;
}
