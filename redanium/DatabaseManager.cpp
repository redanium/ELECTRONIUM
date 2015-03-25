#include "DatabaseManager.h"
#include <QDir>
#include <QSqlQuery>
#include<QDebug>
#include<QFileInfo>
#include<QFile>
#include<QCoreApplication>
DatabaseManager::DatabaseManager(){
    qDebug() << "Database Manager created.";
}
DatabaseManager::~DatabaseManager(){
    qDebug() << "Database Manager destroyed.";
}

bool DatabaseManager::openDB()
{
    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE");
#ifdef Q_OS_ANDROID

// add version checking
    if(QFileInfo("/sdcard/mydatabase.sqlite").exists())

        //in our case we set the existing sqlite3 database file path
    {
        db.setDatabaseName("/sdcard/mydatabase.sqlite");

        qDebug() << "Database file exits :" << QFileInfo(QDir::currentPath().append("/mydatabase.sqlite")).exists();

        qDebug() << "Database Connected." ;
        qDebug() << lastError().text() ;
        return db.open();
    }

    else    {
        QFile::copy(QString(":/files/data/mydatabase.sqlite"),QString("/sdcard/mydatabase.sqlite"));
        db.setDatabaseName("/sdcard/mydatabase.sqlite");

        qDebug() << lastError().text() ;
        return db.open();
    }


#else
    if(QFileInfo(QCoreApplication::applicationDirPath().append("/mydatabase.sqlite")).exists())

        //in our case we set the existing sqlite3 database file path
    {
        db.setDatabaseName(QCoreApplication::applicationDirPath().append("/mydatabase.sqlite"));

        qDebug() << "Database file exits :" << QCoreApplication::applicationDirPath().append("/mydatabase.sqlite")/*.exists()*/;

        qDebug() << "Database Connected." ;

        return db.open();
    }

    else    {
        qDebug() << lastError().text() ;
        return false;
    }
#endif // Q_OS_ANDROID
}

void DatabaseManager::insert(Incident_obj _incident)
{
    QSqlQuery qry;
    QString str_qry( "INSERT INTO INCIDENTS (id, Objet,Agence,Agent,Date,Heure_debut_fin,Convention,Montant,Travaux) VALUES (%1, %2, %3,%4,%5,%6,%7,%8,%10)" );
    qry.prepare(str_qry.arg(_incident.get_i_NumID()).arg(_incident.get_i_Objet()).arg(_incident.get_i_Agence()).arg(_incident.get_i_Agent().join("\n")).arg(_incident.get_i_Date()).arg(_incident.get_i_Heure_debut_fin().first()).arg(_incident.get_i_Convention()).arg(_incident.get_i_Montant())    );
      if( !qry.exec() )
        qDebug() << qry.lastError();
      else
        qDebug( "Inserted!" );


}

QSqlError DatabaseManager::lastError()
{
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
}

bool DatabaseManager::deleteDB()
{
    // Close database
    db.close();

#ifdef Q_OS_LINUX
    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append("my.db.sqlite");
    path = QDir::toNativeSeparators(path);
    return QFile::remove(path);
#else

    // Remove created database binary file
    return QFile::remove("my.db.sqlite");
#endif
}
bool DatabaseManager::create_Incident_Table(){


    // Create table "incident"
    bool ret = false;
    if (db.isOpen())
    {
        QSqlQuery query;
        ret = query.exec(
                    "CREATE TABLE IF NOT EXISTS INCIDENTS "

                    "(id integer unique primary key, "

                    "Objet varchar(150),"

                    "Agence varchar(20), "

                    "Agent varchar(30), "

                    "Date varchar(30),"

                    "Heure_debut_fin varchar(30),"

                    "Convention INTEGER,"

                    "Travaux TEXT,"

                    "Presence TEXT,"

                    "Montant DOUBLE)"

                    );

    }
    return ret;





}
QStringList DatabaseManager::getDbTables(){

    QStringList tableList;

    // QSqlQuery query(db);

    if (db.isOpen())

    {
        qDebug() << "Database has an opening error : "<<db.isOpenError();


        //        if (query.exec("SELECT name FROM sqlite_master WHERE type='table'"))
        //        {
        //            while(query.next())
        //                qDebug()<<tableList<<query.value(0).toString();}

        //        else
        //            qDebug() << query.lastError().text();
        tableList = db.tables();
        tableList.pop_front();
        //tableList.swap(0,tableList.size()-1);

        return tableList;

    }

    else

    { QStringList list = (QStringList() <<"Database Error : "<<"Database File not found.");
        return list;
    }
}

QStringList DatabaseManager::getDictionary()
{
    QSqlTableModel *m_MyModel = new QSqlTableModel(0,db);
    QStringList dictionary;


        m_MyModel->setTable("Actes");
      //  m_MyModel->generateRoleNames();
        m_MyModel->select();
        for(int i=0;i<m_MyModel->rowCount();i++){
            dictionary.append(m_MyModel->data(m_MyModel->index(i,0),Qt::DisplayRole).toString());
        qDebug()<<m_MyModel->data(m_MyModel->index(i,0),Qt::DisplayRole).toString();
    }
    m_MyModel->deleteLater();
    return dictionary;
}






