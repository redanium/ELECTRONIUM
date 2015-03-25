#include "Incident_Travaux.h"
#include<QWebView>
#include"incident.h"

Incident_Travaux::Incident_Travaux(QObject *parent) :QObject(parent)
{
    qDebug("Travaux Class Created");
    MEO.append("#MEO");
    Fournitures.append("#Fournitures");
}

Incident_Travaux::~Incident_Travaux()
{
    qDebug("Travaux Class, THE END");
}

QList<Incident_Travaux::Acte> Incident_Travaux::getMEO() const
{
    return MEO;
}

void Incident_Travaux::setMEO(const QList<Acte> &value)
{
    MEO = value;
}

void Incident_Travaux::ModifyQty(QStandardItem *item)
{

//    Incident_obj* inc=(Incident_obj*)parent();
//QWebView* webview =inc->getI_webView();
//  if(item->model()->objectName().contains("modelF") && Fournitures[Fournitures.size()-item->row()-1].split("#")[1].toInt()>=1)
//  {
//     this->Fournitures[Fournitures.size()-item->row()-1] = QString(this->Fournitures[Fournitures.size()-item->row()-1].split("#")[0]).append(QString("#").append(this->Fournitures[Fournitures.size()-item->row()-1].split("#")[1]).append(QString("#").append(item->data(Qt::DisplayRole).toString()))) ;

////item->model()->setData(item->model()->index(item->row(),4),item->data(Qt::DisplayRole).toInt()*item->model()->data(item->model()->index(item->row(),3)).toDouble(),Qt::DisplayRole);
//  }
//  if(item->model()->objectName().contains("modelM"))
//  {
//     this->MEO[MEO.size()-item->row()-1] = QString(this->MEO[MEO.size()-item->row()-1].split("#")[0]).append(QString("#").append(this->MEO[MEO.size()-item->row()-1].split("#")[1]).append(QString("#").append(item->data(Qt::DisplayRole).toString()))) ;
////item->model()->setData(item->model()->index(item->row(),4),item->data(Qt::DisplayRole).toInt()*item->model()->data(item->model()->index(item->row(),3)).toDouble(),Qt::DisplayRole);

//  }
//  if(webview->title().contains("facture"))   inc->i_Preview(0,webview);
//  if(webview->title().contains("bcmd"))   inc->i_Preview(1,webview);
//  if(webview->title().contains("intern"))   inc->i_Preview(2,webview);
  qDebug()<< this->Fournitures << "hhhhhhhhh" << this->MEO ;
}
QList<Incident_Travaux::Acte> Incident_Travaux::getFournitures() const
{
    return Fournitures;
}

void Incident_Travaux::setFournitures(const QList<Incident_Travaux::Acte> &value)
{
    Fournitures = value;
}

void Incident_Travaux::addActe(int List_number, Incident_Travaux::Acte _acte)
{
    if(List_number==1) //case of MEO LIST
        this->MEO.append(_acte);

    if(List_number==2) //case of Fournitures LIST
        this->Fournitures.append(_acte);
    if(List_number==0) //Both
    {
        this->Fournitures.append(_acte);
        this->MEO.append(_acte);
    }
}

void Incident_Travaux::removeFirst()
{
    this->MEO.pop_front();
    this->Fournitures.pop_front();
}

void Incident_Travaux::deleteAt(int i, int List_number)
{
    if(List_number==1) //case of MEO LIST
        this->MEO.removeAt(i);

    if(List_number==2) //case of Fournitures LIST
        this->Fournitures.removeAt(i);
    if(List_number==0) //Both
    {
        this->Fournitures.removeAt(i);
        this->MEO.removeAt(i);
    }
}

