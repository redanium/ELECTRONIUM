#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H
#include <QFile>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

#include<QDataWidgetMapper>
#include<QSqlTableModel>
#include<QSqlResult>
#include"incident.h"
#include<QSqlQuery>

class DatabaseManager
{


public:
    DatabaseManager();
    ~DatabaseManager();
    bool openDB();
    void insert(Incident_obj _incident);
    void update();
    bool deleteDB();
    bool create_Incident_Table();
    QStringList getDbTables();
    QStringList getDictionary();
    QSqlError lastError();
    inline  QSqlDatabase getDatabase(){return db;}
private:
    QSqlDatabase db;
    QSqlTableModel*             tableModel;
    QDataWidgetMapper*          dataMapper;
};
#endif // DATABASEMANAGER_H
