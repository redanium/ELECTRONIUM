#ifndef ACTE_H
#define ACTE_H

#include <QObject>
#include<QtCore>
#include<QList>
#include<QMap>
#include<QStandardItemModel>

using namespace std;

class Incident_Travaux :public QObject
{
    Q_OBJECT
public slots:
    void ModifyQty(QStandardItem* item);

public:
    explicit  Incident_Travaux(QObject *parent = 0);
    ~Incident_Travaux();

    typedef QString Acte ;
    QList<Acte> getMEO() const;
    void setMEO(const QList<Acte> &value);
    QList<Acte> getFournitures() const;
    void setFournitures(const QList<Acte> &value);

    void addActe(int List_number ,Acte _acte);
    void removeFirst();
    void deleteAt(int i,int List_number);
    QStringList getAll(){

        QStringList str;
        if(this->MEO.contains("MEO"))
            this->MEO.pop_front();
        if(this->Fournitures.contains("Fournitures"))
            this->Fournitures.pop_front();

        str.append( this->MEO) ;
        str .append( this->Fournitures);
        return str;
    }

private:


    QList<Acte> MEO;
    QList<Acte> Fournitures;

};

#endif // ACTE_H
