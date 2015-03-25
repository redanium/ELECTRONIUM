#include<QFile>
#include"incident.h"
#include<QTextDocument>
#include<QtPrintSupport/QPrinter>
#include<QFileDialog>
#include<QPrintPreviewDialog>
#include<QTextStream>
#include<QDebug>
#include<QList>
#include<QDate>

#include<QtWidgets>
#include"Incident_Travaux.h"
#include"Form_Incident.h"


int Incident_obj::get_i_Commande()
{
    return i_Commande;
}

void Incident_obj::set_i_Commande( int value)
{
    i_Commande = value;
}

void Incident_obj::addAgent(QString agentName)
{
    i_Agent.append(agentName);
 if(i_webView->title().contains("facture"))   i_Preview(0,i_webView);
 if(i_webView->title().contains("bcmd"))   i_Preview(1,i_webView);
 if(i_webView->title().contains("intern"))   i_Preview(2,i_webView);
}

void Incident_obj::removeAgent(QObject *agentObj)
{
    for (int i = 0; i < i_Agent.size(); ++i) {
        if(i_Agent.at(i).contains(agentObj->objectName()))
            i_Agent.removeAt(i);
    }
    if(i_webView->title().contains("facture"))   i_Preview(0,i_webView);
    if(i_webView->title().contains("bcmd"))   i_Preview(1,i_webView);
    if(i_webView->title().contains("intern"))   i_Preview(2,i_webView);

}

Incident_obj::Incident_obj(QObject *parent) :QObject(parent)
{
    QSettings settings("Redanium", "Oso Manager");

    qDebug()<<  "new incident object";
    i_Travaux= new Incident_Travaux(this);
    i_NumID=0;
    i_Convention=settings.value("incident/convention_année").toInt();
    i_Direction= "Direction";
    i_Montant=0;
    i_Commande=0;
    i_Date="00/00/0000";
    i_Date_Cmd="00/00/0000";
    i_Date_Facture="00/00/0000";
    i_heure_debut_fin<<"00:00"<<"00:00";
    i_Objet="Objet";
    i_Agence="Agence";
    //i_Agent="pas d'agent";
    majoration_Fifty_DayOff=false;
    majoration_Fifty_Night=false;
    imputation=Incident_obj::AERIEN;
    i_numero_facture=0;
    Type=Form_incident::INCIDENT;
    i_coordinate = "35.6994853,-0.6416702";

    // i_Presence<<"pas de présence";


}

Incident_obj::~Incident_obj()
{
    qDebug()<<"the end of incident object";
}
QWebView *Incident_obj::getI_webView()
{
    return i_webView;
}

void Incident_obj::setI_webView(QWebView *value)
{
    i_webView = value;
}


void Incident_obj::QtyFournituresChanged(QStandardItem *item){
    qDebug()<<"item row"<<item->data(Qt::DisplayRole).toString();
    
    qDebug()<<this->i_fournitures.keys();
    if(item->index().column()==1){
        qDebug()<<"item row inside"<<item->data(Qt::DisplayRole).toString();

        this->i_fournitures[item->model()->data(item->model()->index(item->index().row(),0),Qt::DisplayRole).toString()]=item->data(Qt::DisplayRole).toInt();
    }
    if(i_webView->title().contains("facture"))   i_Preview(0,i_webView);
    if(i_webView->title().contains("bcmd"))   i_Preview(1,i_webView);
    if(i_webView->title().contains("intern"))   i_Preview(2,i_webView);
}
QMap<QString,int> Incident_obj::getI_fournitures()
{
    return i_fournitures;
}

void Incident_obj::setI_fournitures( QMap<QString,int> &value)
{
    i_fournitures = value;
}

int Incident_obj::getType() const
{
    return Type;
}

void Incident_obj::setType(int value)
{
    Type = value;
}


void Incident_obj::i_Preview(int type, QWebView *webview)
{

    QSettings settings("Redanium", "Oso Manager");
    double montantHT=0,montantMeoHT=0;
    QStringList htmlActeMeo,htmlActeF;
    QString t_str;

//((Form_incident*)parent())->updateMoney();
    QString Presence_Fournitures_Row = "<tr>"
            "<td>%1</td>"
            "<td>%2</td>"
            "</tr>"

            ;
    QString majorationRowBoth="<tr >"
            "   <td style=\"margin-left:5px\" ><b>&nbsp;Majoration 50% (Jour férier/Travail de nuit)</b></td>"
            "  <td align=\"center\"><b>/</b></td>"
            "    <td align=\"center\"><b>/</b></td>"
            "  <td align=\"center\"><b>%23</b></td>"
            " <td align=\"right\"><b>%23&nbsp;</b></td>"
            "</tr>";
    QString majorationRowDoff="<tr >"
            "   <td style=\"margin-left:5px\" ><b>&nbsp;Majoration 50% (Jour férier)</b></td>"
            "  <td align=\"center\"><b>/</b></td>"
            "    <td align=\"center\"><b>/</b></td>"
            "  <td align=\"center\"><b>%23</b></td>"
            " <td align=\"right\"><b>%23&nbsp;</b></td>"
            "</tr>";
    QString majorationRowNight="<tr >"
            "   <td style=\"margin-left:5px\" ><b>&nbsp;Majoration 50% (Travail de nuit)</b></td>"
            "  <td align=\"center\"><b>/</b></td>"
            "    <td align=\"center\"><b>/</b></td>"
            "  <td align=\"center\"><b>%23</b></td>"
            " <td align=\"right\"><b>%23&nbsp;</b></td>"
            "</tr>";
    QString MeoHeader ="<tr class=\"odd\">"
            "<td colspan=\"5\" bgcolor=\"lightgrey\"><b>&nbsp;Mise en Oeuvre</b></td>"
            "</tr>";
    QString FournituresHeader ="<tr class=\"odd\">"
            "<td colspan=\"5\" bgcolor=\"lightgrey\"><b>&nbsp;Fournitures</b></td>"
            "</tr>";


    QString strActe="<tr >"
            "<td>&nbsp;%1</td>"
            "<td align=\"center\">%2</td>"
            "<td align=\"center\">%3</td>"
            "<td align=\"center\">%4</td>"
            "<td align=\"right\">%5&nbsp;</td>"
            "</tr>";

    QString head_b_cmd ="<tr><td colspan=3><b>&nbsp;%1</b></td></tr>";



    QString b_cmd_acte= "<tr>"
            "<td align=\"left\">&nbsp;%1</td>"
            "<td align=\"center\">%2</td>"
            "<td align=\"right\">%3&nbsp;</td>"
            "</tr>";

    QFile filehtml;
    //open the html template from ressources
    if(type==0)filehtml.setFileName(":/files/data/template.html");
    if(type==1)filehtml.setFileName(":/files/data/bon_commande_.htm");
    if(type==2)filehtml.setFileName(":/files/data/interne.html");


    if (filehtml.open(QFile::ReadOnly | QFile::Text))
        qDebug()<<"template.html is open : "<<filehtml.isOpen();
    QTextStream in(&filehtml);
    QString str = QString(in.readAll());
    webview->setHtml(str);
    filehtml.close();
    if(type==0){
        QAbstractItemModel *model_f = parent()->findChild<QTableView*>("tableView_Fournitures")->model();
        if(model_f!=0)
        {
            if(this->getI_Travaux()->getFournitures().size()>1){
                htmlActeF.append(FournituresHeader);
                for (int i = 0; i < model_f->rowCount(); ++i) {
                    t_str=strActe.arg(model_f->data(model_f->index(i,0)).toString())
                            .arg(model_f->data(model_f->index(i,1)).toString())
                            .arg(model_f->data(model_f->index(i,2)).toString())
                            .arg(model_f->data(model_f->index(i,3)).toString())
                            .arg(model_f->data(model_f->index(i,4)).toString());
                    htmlActeF.append(t_str);
                    montantHT = montantHT + model_f->data(model_f->index(i,4)).toDouble();

                }

            }
        }
        QAbstractItemModel *model_m = parent()->findChild<QTableView*>("tableView_MEO")->model();
        if(model_m!=0)
        {
            if(this->getI_Travaux()->getMEO().size()>1){
                htmlActeMeo.append(MeoHeader);
                for (int i = 0; i < model_m->rowCount(); ++i) {
                    t_str=strActe.arg(model_m->data(model_m->index(i,0)).toString())
                            .arg(model_m->data(model_m->index(i,1)).toString())
                            .arg(model_m->data(model_m->index(i,2)).toString())
                            .arg(model_m->data(model_m->index(i,3)).toString())
                            .arg(model_m->data(model_m->index(i,4)).toString());
                    htmlActeMeo.append(t_str);
                    montantHT = montantHT + model_m->data(model_m->index(i,4)).toDouble();
                    montantMeoHT = montantMeoHT + model_m->data(model_m->index(i,4)).toDouble();
                }

                if(this->Type==Form_incident::INCIDENT)  {
                    if(this->getMajoration_Fifty_DayOff()==this->getMajoration_Fifty_Night()){
                        if(this->getMajoration_Fifty_DayOff()) {
                            htmlActeMeo.append(majorationRowBoth);
                            montantHT=montantHT+0.5*montantMeoHT;
                        }
                    }
                    else{
                        if(this->getMajoration_Fifty_DayOff()) {
                            htmlActeMeo.append(majorationRowDoff);
                            montantHT=montantHT+0.5*montantMeoHT;

                        }
                        if(this->getMajoration_Fifty_Night()){
                            htmlActeMeo.append(majorationRowNight);
                            montantHT=montantHT+0.5*montantMeoHT;

                        }
                    }
                }

            }
        }



        webview->setHtml( // 1
                          str.arg(this->getI_Date_Facture())
                          // 2
                          .arg(this->getI_Direction())
                          // 3
                          .arg(this->get_i_Agence())
                          .arg(QString(this->get_i_Objet()).prepend(((Form_incident*)parent())->getF_form_type()==Form_incident::INCIDENT?"Intitul&eacute; de l'incident : ":"Intitul&eacute; de l'entretien : "))
                          // 4
                          .arg(this->get_i_Date())
                          // 5
                          .arg(Incident_obj::ChiffreEnLettre((montantHT-montantHT*0.05)*0.17+montantHT-montantHT*0.05))
                          //6
                          .arg(htmlActeMeo.join("<!-- -->"))
                          //7
                          .arg(htmlActeF.join("<!-- -->"))
                          //8
                          .arg(settings.value(((Form_incident*)parent())->getF_form_type()==Form_incident::INCIDENT?"incident/convention_numero":"entretien/convention_numero").toInt())
                          //9
                          .arg(settings.value(((Form_incident*)parent())->getF_form_type()==Form_incident::INCIDENT?"incident/convention_année":"entretien/convention_année").toInt())
                          //10
                          .arg(((Form_incident*)parent())->getF_form_type()==Form_incident::INCIDENT?QString::number(this->getImputation()).prepend("IMPUTATION 932 1C1 "):QString())
                          //11
                          .arg(((Form_incident*)parent())->getF_form_type()==Form_incident::INCIDENT?QString("BON DE COMMANDE N&deg; %1").arg(QString::number(this->get_i_Commande()).append(" du %13")):QString())
                          //12
                          .arg(this->getI_Date_Cmd())
                          //13
                          .arg(this->getI_numero_facture())
                          //14
                          .arg(" ")
                          //15
                          .arg(QString::number(montantHT,'f',2))
                          //16
                          .arg(QString::number(montantHT*0.05,'f',2))
                          //17
                          .arg(QString::number(montantHT-montantHT*0.05,'f',2))
                          //18
                          .arg(QString::number((montantHT-montantHT*0.05)*0.17,'f',2))
                          //19
                          .arg(QString::number((montantHT-montantHT*0.05)*0.17+montantHT-montantHT*0.05,'f',2))

                          //20
                          .arg(this->get_i_Agent().isEmpty()?" ":this->get_i_Agent().at(0))
                          //21
                          .arg((this->get_i_Agent().size()<=1 )? " ":this->get_i_Agent().at(1))
                          //22
                          .arg(QString::number(montantMeoHT*0.5,'f',2))
                          );

    }

    if(type==1){
        QAbstractItemModel *model_f = parent()->findChild<QTableView*>("tableView_Fournitures")->model();
        if(model_f!=0)
        {
            if(this->getI_Travaux()->getFournitures().size()>1){
                htmlActeF.append(head_b_cmd.arg("Fournitures"));
                for (int i = 0; i < model_f->rowCount(); ++i) {
                    t_str=b_cmd_acte.arg(model_f->data(model_f->index(i,0)).toString())
                            .arg(model_f->data(model_f->index(i,1)).toString())
                            .arg(model_f->data(model_f->index(i,3)).toString())
                            ;
                    htmlActeF.append(t_str);
                    montantHT = montantHT + model_f->data(model_f->index(i,4)).toDouble();

                }

            }
        }
        QAbstractItemModel *model_m = parent()->findChild<QTableView*>("tableView_MEO")->model();
        if(model_m!=0)
        {
            if(this->getI_Travaux()->getMEO().size()>1){
                htmlActeMeo.append(head_b_cmd.arg("Mise en Oeuvre"));
                for (int i = 0; i < model_m->rowCount(); ++i) {
                    t_str=b_cmd_acte.arg(model_m->data(model_m->index(i,0)).toString())
                            .arg(model_m->data(model_m->index(i,1)).toString())
                            .arg(model_m->data(model_m->index(i,3)).toString())
                            ;
                    htmlActeMeo.append(t_str);
                    montantHT = montantHT + model_m->data(model_m->index(i,4)).toDouble();
                    montantMeoHT = montantMeoHT + model_m->data(model_m->index(i,4)).toDouble();
                }

                if(this->Type==Form_incident::INCIDENT) {
                    if(this->getMajoration_Fifty_DayOff()==this->getMajoration_Fifty_Night()){
                        if(this->getMajoration_Fifty_DayOff()) {
                            htmlActeMeo.append(b_cmd_acte.arg("Majoration 50% (Jour férier/Travail de nuit)").arg("/").arg(QString::number(montantMeoHT*0.5,'f',2)));
                            montantHT=montantHT+0.5*montantMeoHT;
                        }
                    }
                    else{
                        if(this->getMajoration_Fifty_DayOff()) {
                            htmlActeMeo.append(b_cmd_acte.arg("Majoration 50% (Jour férier)").arg("/").arg(QString::number(montantMeoHT*0.5,'f',2)));
                            montantHT=montantHT+0.5*montantMeoHT;

                        }
                        if(this->getMajoration_Fifty_Night()){
                            htmlActeMeo.append(b_cmd_acte.arg("Majoration 50% (Travail de nuit)").arg("/").arg(QString::number(montantMeoHT*0.5,'f',2)));
                            montantHT=montantHT+0.5*montantMeoHT;

                        }
                    }
                }

            }
        }
        webview->setHtml( // 1
                          str.arg(this->get_i_Commande())
                          // 2
                          .arg(settings.value("incident/convention_année").toInt())
                          // 3
                          .arg(this->getI_Date_Cmd())
                          // 4
                          .arg(settings.value(QString("entreprise/nom")).toString())
                          // 5
                          .arg(this->get_i_Objet())
                          //6
                          .arg(this->get_i_Date())
                          //7
                          .arg(" ")
                          //8
                          .arg(this->getImputation())
                          //9
                          .arg(htmlActeMeo.join("<!-- -->"))
                          //10
                          .arg(htmlActeF.join("<!-- -->"))
                          //11
                          .arg(QString::number(montantHT,'f',2))
                          //12
                          .arg(QString::number(montantHT*0.17,'f',2))
                          //13
                          .arg(QString::number(montantHT*1.17,'f',2))
                          //14
                          .arg(Incident_obj::ChiffreEnLettre(montantHT*1.17))
                          //15
                          .arg(settings.value(QString("entreprise/r_id_bank")           ).toString())
                          //                          //16
                          //                          .arg(QString::number(montantHT*0.05,'f',2))
                          //                          //17
                          //                          .arg(QString::number(montantHT-montantHT*0.05,'f',2))
                          //                          //18
                          //                          .arg(QString::number((montantHT-montantHT*0.05)*0.17,'f',2))
                          //                          //19
                          //                          .arg(QString::number((montantHT-montantHT*0.05)*0.17+montantHT-montantHT*0.05,'f',2))
                          //                           //20
                          //                          .arg(QString::number(montantMeoHT*0.5,'f',2))

                          );

    }
    if(type==2){
        QAbstractItemModel *model_f = parent()->findChild<QTableView*>("tableView_Fournitures")->model();
        if(model_f!=0)
        {
            if(this->getI_Travaux()->getFournitures().size()>1){
                htmlActeF.append(FournituresHeader);
                for (int i = 0; i < model_f->rowCount(); ++i) {
                    t_str=strActe.arg(model_f->data(model_f->index(i,0)).toString())
                            .arg(model_f->data(model_f->index(i,1)).toString())
                            .arg(model_f->data(model_f->index(i,2)).toString())
                            .arg(model_f->data(model_f->index(i,3)).toString())
                            .arg(model_f->data(model_f->index(i,4)).toString());
                    htmlActeF.append(t_str);
                    montantHT = montantHT + model_f->data(model_f->index(i,4)).toDouble();

                }

            }
        }
        QAbstractItemModel *model_m = parent()->findChild<QTableView*>("tableView_MEO")->model();
        if(model_m!=0)
        {
            if(this->getI_Travaux()->getMEO().size()>1){
                htmlActeMeo.append(MeoHeader);
                for (int i = 0; i < model_m->rowCount(); ++i) {
                    t_str=strActe.arg(model_m->data(model_m->index(i,0)).toString())
                            .arg(model_m->data(model_m->index(i,1)).toString())
                            .arg(model_m->data(model_m->index(i,2)).toString())
                            .arg(model_m->data(model_m->index(i,3)).toString())
                            .arg(model_m->data(model_m->index(i,4)).toString());
                    htmlActeMeo.append(t_str);
                    montantHT = montantHT + model_m->data(model_m->index(i,4)).toDouble();
                    montantMeoHT = montantMeoHT + model_m->data(model_m->index(i,4)).toDouble();
                }

                if(this->Type==Form_incident::INCIDENT) {
                    if(this->getMajoration_Fifty_DayOff()==this->getMajoration_Fifty_Night()){
                        if(this->getMajoration_Fifty_DayOff()) {
                            htmlActeMeo.append(majorationRowBoth);
                            montantHT=montantHT+0.5*montantMeoHT;
                        }
                    }
                    else{
                        if(this->getMajoration_Fifty_DayOff()) {
                            htmlActeMeo.append(majorationRowDoff);
                            montantHT=montantHT+0.5*montantMeoHT;

                        }
                        if(this->getMajoration_Fifty_Night()){
                            htmlActeMeo.append(majorationRowNight);
                            montantHT=montantHT+0.5*montantMeoHT;

                        }
                    }
                }

            }
        }QString presenceTmp,FournituresTmp;
        for (int i = 0; i < this->getI_Presence().size(); ++i) {
            presenceTmp.append(Presence_Fournitures_Row.arg(this->getI_Presence().at(i).split("#").at(0)).arg(this->getI_Presence().at(i).split("#").at(1)));
        }
        foreach (QString key, this->getI_fournitures().keys()) {
            FournituresTmp.append(Presence_Fournitures_Row.arg(key).arg(this->getI_fournitures()[key]));

        }

        webview->setHtml( // 1
                          str.arg(this->get_i_Objet())
                          // 2
                          .arg(this->get_i_Date())
                          // 3
                          .arg(this->getI_Direction())
                          // 4
                          .arg(this->get_i_Agence())
                          // 5
                          .arg(this->get_i_Commande())
                          //6
                          .arg(this->getI_Date_Cmd())
                          //7
                          .arg(settings.value("incident/convention_année").toInt())
                          //8
                          .arg(htmlActeMeo.join("<!-- -->"))
                          //9
                          .arg(htmlActeF.join("<!-- -->"))
                          //10
                          .arg(QString::number(montantHT,'f',2))
                          //11
                          .arg(QString::number(montantHT*0.05,'f',2))
                          //12
                          .arg(QString::number(montantHT-montantHT*0.05,'f',2))
                          //13
                          .arg(QString::number((montantHT-montantHT*0.05)*0.17,'f',2))
                          //14
                          .arg(QString::number((montantHT-montantHT*0.05)*0.17+montantHT-montantHT*0.05,'f',2))
                          //15
                          .arg(Incident_obj::ChiffreEnLettre(montantHT*1.17))
                          //16
                          .arg(this->get_i_Heure_debut_fin().isEmpty()?"00:00":this->get_i_Heure_debut_fin().at(0))
                          //17
                          .arg(this->get_i_Heure_debut_fin().isEmpty()?"00:00":this->get_i_Heure_debut_fin().at(1))
                          //18
                          .arg(this->getMajoration_Fifty_DayOff()? (this->getMajoration_Fifty_Night()?"Jour Férier/Travail de Nuit":"Jour Férier "):(this->getMajoration_Fifty_Night()?"Travail de Nuit":" "))
                          //19
                          .arg(this->get_i_Agent().join(" ## "))
                          //20
                          .arg(presenceTmp)
                          //21
                          .arg(FournituresTmp)
                          //22
                          .arg(this->getI_notes().join("</br>"))
                          );
        qDebug()<<"======================================"<<FournituresTmp << this->getI_fournitures();
    }


}
QStringList Incident_obj::getI_notes() const
{
    return i_notes;
}

void Incident_obj::setI_notes(const QStringList &value)
{
    i_notes = value;
}

QString Incident_obj::getI_Direction() const
{
    return i_Direction;
}

void Incident_obj::setI_Direction(const QString &value)
{
    i_Direction = value;
}



QString Incident_obj::getI_coordinate() const
{
    return i_coordinate;
}

void Incident_obj::setI_coordinate(const QString &value)
{
    i_coordinate = value;
    qDebug()<< "GeoCoordinates from JavaScript" << i_coordinate;
}

int Incident_obj::getI_numero_facture() const
{
    return i_numero_facture;
}

void Incident_obj::setI_numero_facture(int value)
{
    i_numero_facture = value;
}

bool Incident_obj::getMajoration_Fifty_DayOff() const
{
    return majoration_Fifty_DayOff;
}

void Incident_obj::setMajoration_Fifty_DayOff(bool value)
{
    majoration_Fifty_DayOff = value;
}

bool Incident_obj::getMajoration_Fifty_Night() const
{
    return majoration_Fifty_Night;
}

void Incident_obj::setMajoration_Fifty_Night(bool value)
{
    majoration_Fifty_Night = value;
}

int Incident_obj::getImputation() const
{
    return imputation;
}

void Incident_obj::setImputation(int value)
{
    imputation = value;
}

void Incident_obj::i_print()
{
    qDebug()<<"Numero "<<i_NumID; //auto_increment + intern
    qDebug()<<"Objet "<< i_Objet;
    qDebug()<<"Agence "<<i_Agence ;
    qDebug()<<"Agent "<< i_Agent;
    qDebug()<<"date incident "<< i_Date;
    qDebug()<<"date facture"<< i_Date_Facture;//date facture changeable
    qDebug()<<"date cmd "<< i_Date_Cmd;
    qDebug()<<"cmd "<< i_Commande; // Nxx/DTE/yyyy
    qDebug()<<"H D-F "<< i_heure_debut_fin;//majoration 50% sur MEO , majoration travail de nuit : 50% ##jours feries
    qDebug()<<"convention "<< i_Convention;
    qDebug()<<"Presence "<< i_Presence;
    qDebug()<<"Actes "<<i_Travaux->getMEO();
    qDebug()<<"Actes "<<i_Travaux->getFournitures();
    qDebug()<<"Montant "<< i_Montant;
    qDebug()<< "Imputation "<<imputation;
    qDebug()<< "GeoCoordinate"<<i_coordinate;
    qDebug()<< "Notes "<<i_notes;
}

QString Incident_obj::getI_Date_Cmd() const
{
    return i_Date_Cmd;
}

void Incident_obj::setI_Date_Cmd(const QString &value)
{
    i_Date_Cmd = value;
}

QString Incident_obj::getI_Date_Facture() const
{
    return i_Date_Facture;
}

void Incident_obj::setI_Date_Facture(const QString &value)
{
    i_Date_Facture = value;
}

QStringList Incident_obj::getI_Presence() const
{
    return i_Presence;
}

void Incident_obj::setI_Presence(const QStringList &value)
{
    i_Presence = value;
}


Incident_Travaux *Incident_obj::getI_Travaux()
{
    return i_Travaux;
}

void Incident_obj::setI_Travaux(Incident_Travaux *value)
{
    i_Travaux = value;
}
void Incident_obj::prepareData4Plot(QAbstractItemModel *model,QVector<QString> x,QVector<double> y)
{
    double montant_convention = 8000000;

    model->sort(3,Qt::AscendingOrder);

    for (int i = 0; i < model->rowCount(); ++i) {
        x[i]=model->data(model->index(i,3),Qt::DisplayRole).toString();
        montant_convention=montant_convention-model->data(model->index(i,12),Qt::DisplayRole).toDouble();
        y[i]=montant_convention;
        qDebug() << "montant :"<<montant_convention;
        qDebug() << "date :"<<x[i];
        qDebug() << "valeur :"<<y[i];
    }


}



bool Incident_obj::i_Serialize(QString filepath)
{
    QFile file(filepath);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    out.setVersion(QDataStream::Qt_5_1);
    out << this->i_Agence;
    out << this->i_Agent;
    out << this->i_Date;
    out << this->i_Date_Cmd;
    out << this->i_Date_Facture;
    out << this->i_heure_debut_fin;
    out << this->i_Convention;
    out << this->i_Montant;
    out << this->i_Objet;
    out << this->i_NumID;
    out << this->imputation;
    out << this->i_Commande;
    out << this->i_Presence;

    out << this->i_Travaux->getMEO();
    out << this->i_Travaux->getFournitures();
    out << this->i_notes;
    out << this->Type;



    file.close();
    return true;
    
}

void Incident_obj::i_DeSerialize(QString filepath)
{



    QFile file(filepath);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);    // read the data serialized from the file
    in.setVersion(QDataStream::Qt_5_1);
    in >> this->i_Agence;
    in >> this->i_Agent;
    in >> this->i_Date;
    in >> this->i_Date_Cmd;
    in >> this->i_Date_Facture;
    in >> this->i_heure_debut_fin;
    in >> this->i_Convention;
    in >> this->i_Montant;
    in >> this->i_Objet;
    in >> this->i_NumID;
    in >> this->imputation;
    in >> this->i_Commande;
    in >> this->i_Presence;


    QStringList lista;
    in >> lista;//acte;
    this->i_Travaux->setMEO(lista);

    lista.clear();
    in >> lista;
    this->i_Travaux->setFournitures(lista);

    lista.clear();
    in >> lista;
    this->setI_notes(lista);

    in >> this->Type ;
    file.close();
}

const QString Incident_obj::ChiffreEnLettre(double montant)
{
    QString lettre;
    int j=0;
    
    QString montant_string = QString::number(montant,'f',2);
    
    QStringList montant_str_list = montant_string.split(".");
    
    
    QList<QString>::iterator i;
    
    for (i=montant_str_list.begin();i!=montant_str_list.end();i++)
    {
        
        
        QString str_tmp =*i;
        
        int  montant = (str_tmp).toInt();
        
        
        unsigned int centaine, dizaine, unite, reste, y;
        
        bool dix = false;
        
        reste = montant;
        
        for(int i=1000000000; i>=1; i/=1000)
            
        {
            
            y = reste/i;
            
            
            if(y!=0)
                
            {
                centaine = y/100;
                
                dizaine  = (y - centaine*100)/10;
                
                unite = y-(centaine*100)-(dizaine*10);
                switch(centaine)
                {
                case 0:
                    break;
                case 1:
                    lettre.append("cent ");
                    break;
                case 2:
                    if((dizaine == 0)&&(unite == 0)) lettre.append("deux cents ");
                    else lettre.append("deux cent ");
                    break;
                case 3:
                    if((dizaine == 0)&&(unite == 0)) lettre.append("trois cents ");
                    else lettre.append("trois cent ");
                    break;
                case 4:
                    if((dizaine == 0)&&(unite == 0)) lettre.append("quatre cents ");
                    else lettre.append("quatre cent ");
                    break;
                case 5:
                    if((dizaine == 0)&&(unite == 0)) lettre.append("cinq cents ");
                    else lettre.append("cinq cent ");
                    break;
                case 6:
                    if((dizaine == 0)&&(unite == 0)) lettre.append("six cents ");
                    else lettre.append("six cent ");
                    break;
                case 7:
                    if((dizaine == 0)&&(unite == 0)) lettre.append("sept cents ");
                    else lettre.append("sept cent ");
                    break;
                case 8:
                    if((dizaine == 0)&&(unite == 0)) lettre.append("huit cents ");
                    else lettre.append("huit cent ");
                    break;
                case 9:
                    if((dizaine == 0)&&(unite == 0)) lettre.append("neuf cents ");
                    else lettre.append("neuf cent ");
                }// endSwitch(centaine)
                
                switch(dizaine)
                {
                case 0:
                    break;
                case 1:
                    dix = true;
                    break;
                case 2:
                    lettre.append("vingt ");
                    break;
                case 3:
                    lettre.append("trente ");
                    break;
                case 4:
                    lettre.append("quarante ");
                    break;
                case 5:
                    lettre.append("cinquante ");
                    break;
                case 6:
                    lettre.append("soixante ");
                    break;
                case 7:
                    dix = true;
                    lettre.append("soixante ");
                    break;
                case 8:
                    lettre.append("quatre-vingt ");
                    break;
                case 9:
                    dix = true;
                    lettre.append("quatre-vingt ");
                } // endSwitch(dizaine)
                
                switch(unite)
                {
                case 0:
                    if(dix) lettre.append("dix ");
                    break;
                case 1:
                    if(dix) lettre.append("onze ");
                    else    lettre.append("un ");
                    break;
                case 2:
                    if(dix) lettre.append("douze ");
                    else    lettre.append("deux ");
                    break;
                case 3:
                    if(dix) lettre.append("treize ");
                    else    lettre.append("trois ");
                    break;
                case 4:
                    if(dix) lettre.append("quatorze ");
                    else    lettre.append("quatre ");
                    break;
                case 5:
                    if(dix) lettre.append("quinze ");
                    else    lettre.append("cinq ");
                    break;
                case 6:
                    if(dix) lettre.append("seize ");
                    else    lettre.append("six ");
                    break;
                case 7:
                    if(dix) lettre.append("dix-sept ");
                    else    lettre.append("sept ");
                    break;
                case 8:
                    if(dix) lettre.append("dix-huit ");
                    else    lettre.append("huit ");
                    break;
                case 9:
                    if(dix) lettre.append("dix-neuf ");
                    else    lettre.append("neuf ");
                } // endSwitch(unite)
                
                switch (i)
                {
                case 1000000000:
                    if(y>1) lettre.append("milliards ");
                    else lettre.append("milliard ");
                    break;
                case 1000000:
                    if(y>1) lettre.append("millions ");
                    else lettre.append("million ");
                    break;
                case 1000:
                    lettre.append("mille ");
                }
                
            } // end if(y!=0)
            
            reste -= y*i;
            
            dix = false;
            
            
        } // end for
        
        
        if(lettre.size()==0) lettre="zero ";
        
        if(montant_str_list.at(1).toDouble()>0 && j==0) lettre.append("dinars et ");
        if(montant_str_list.at(1).toDouble()==0 && j==1) lettre.append("dinars .");
        j++;

    }
    
    if(montant_str_list.at(1).toDouble()>0) lettre.append("centimes.");
    if(lettre.contains("un mille"))
    { /*lettre[0]='';lettre[1]='';*/
        lettre.replace("un mille","mille");
    }
    lettre[0]=lettre[0].toUpper();
    
    return QString(lettre);
}

QString Incident_obj::get_i_Agence(){
    
    return Incident_obj::i_Agence;
}

void Incident_obj::set_i_Agence(QString agence){
    
    Incident_obj::i_Agence = agence;
}

QStringList Incident_obj::get_i_Agent(){
    return Incident_obj::i_Agent;
}

void Incident_obj::set_i_Agent(QStringList agent){
    Incident_obj::i_Agent=agent;
}

QString Incident_obj::get_i_Date(){
    return this->i_Date;
}

void Incident_obj::set_i_Date(QString date){
    this->i_Date=date;
}

QStringList Incident_obj::get_i_Heure_debut_fin(){
    return this->i_heure_debut_fin;
}

void Incident_obj::set_i_Heure_debut_fin(QString i_heure,int pos){
    this->i_heure_debut_fin.removeAt(pos);
    this->i_heure_debut_fin.insert(pos,i_heure);
}

QString Incident_obj::get_i_Objet(){
    return this->i_Objet;
}

void Incident_obj::set_i_Objet(QString objet){
    this->i_Objet=objet;
}

int Incident_obj::Incident_print(QWidget *parent){
    
    QTextDocument *Incident_document = new QTextDocument();

    QString fileName = QFileDialog::getSaveFileName(parent, "Export PDF",
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");



        QFile filehtml(":/files/data/bill_template.html");


        qDebug()<<"Bill_template.html is open : "<<filehtml.isOpen();

        if (!filehtml.open(QFile::ReadOnly | QFile::Text))
            qDebug()<<"Bill_template.html is open : "<<filehtml.errorString();


        QTextStream in(&filehtml);

        QString str=QString(in.readAll()).arg(QDate::currentDate().toString("dd/MM/yyyy")).arg("TEST").arg("2012").arg("DTE");

        Incident_document->setHtml(str);

        QPrinter printer(QPrinter::HighResolution);

        printer.setPageSize(QPrinter::A4);

        printer.setOutputFormat(QPrinter::PdfFormat);

        printer.setOutputFileName(fileName);

        Incident_document->print(&printer);

        delete Incident_document;

        return 1;
    }
    else
        return 0;
    
}
