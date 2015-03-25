#ifndef FORM_INCIDENT_H
#define FORM_INCIDENT_H
#include "ui_form_incident.h"


#include <QWidget>
#include"incident.h"
#include"mainwindow.h"
#include<QSqlTableModel>
#include<QGroupBox>
#include"i_table_fixedrow.h"
#include"QProgressIndicator.h"

namespace Ui {
class Form_incident;
}
class DatabaseManager;
class MainWindow;
class i_table_fixedrow;

class Form_incident : public QWidget
{
    Q_OBJECT

public:
    enum f_TYPE {INCIDENT=0,ENTRETIEN=1}; //

    explicit Form_incident(QWidget *parent = 0,int form_type= Form_incident::INCIDENT);
    ~Form_incident();
    void tmp_fct();
    int getF_form_type() const;
    void setF_form_type(int value);

public slots:
    void updateMoney();

    Incident_obj* getIncident(){ return tmp_incident;}
    void setIncident(Incident_obj* inc){tmp_incident=inc;}
    QString loadFromFile();
    void loadFromDatabase(int row);
    void update_Ui_Load();
    void updatePreview(int type);
    void updatePreview_();//QModelIndex,QModelIndex to update the Travaux class
    QString saveToFile();
    void saveToDatabase(){}
    void loadStaticMap(bool status);
    void on_spinBox_Cmd_Num_EditingFinished();
    void hideHbox(QHBoxLayout *layout);
    void  updateNotes();

    //void print(){}// we have preview,pdf and printer(hardware)

private slots:

    void on_comboBox_Agent_currentIndexChanged(const QString &arg1);

    void on_dateEdit_editingFinished();

    void on_lineEdit_Objet1_editingFinished();

    void on_tableView_presence_doubleClicked(const QModelIndex &index);

    void on_pushButton_clicked();

    void on_comboBox_Agent_editTextChanged(const QString &arg1);

    void on_dateEdit_2_editingFinished();

    void on_timeEdit_Debut_editingFinished();

    void on_timeEdit_Fin_editingFinished();

    void on_toolButton_2_clicked();

    void on_toolButton_clicked();

    void on_comboBox_2_currentIndexChanged(int index);

    void on_comboBox_Agence_currentIndexChanged(int index);

    void on_toolButton_3_clicked();

    void on_lineEdit_Objet2_textChanged(QString arg);
    void on_textEdit_textChanged();

    void on_toolButton_5_clicked();

    void on_comboBox_3_currentIndexChanged(const QString &arg1);

signals:
    bool Loaded();
    bool saved();
    bool hasChanged();

private:
    Ui::Form_incident *ui;
    i_table_fixedrow *iTableFixRow ;
    Incident_obj *tmp_incident;
    DatabaseManager *manager;
    MainWindow *i_MainWindow;
    QStringList Oran_wilaya;
    int F_form_type;



};
class EmployerModel : public QSqlTableModel {

    Q_OBJECT

public:

    int IMAGE_MODEL ;


    explicit    EmployerModel(QObject * parent = 0, QSqlDatabase db = QSqlDatabase() ,int withIMAGE=0):
        QSqlTableModel(parent, db)
    {
        IMAGE_MODEL= withIMAGE;
        setObjectName("model_employer");
    }

    QVariant data(const QModelIndex &index, int role = Qt::DecorationRole) const{
        QString html =
                "<table border=\"0\">"
                "<tr> <td bgcolor=\"#FFCC00\" align='center'><b>Objet</b></td>  <td align='center'>%1</td>  </tr>"
                "<tr> <td align='center'><b>Date incident</b></td>   <td align='center'>%2</td>                </tr>"
                "<tr> <td bgcolor=\"#FFCC00\" align='center'><b>Date Commande</b></td>       <td align='center'>%3</td>                       </tr>"
                "<tr> <td align='center'><b>Date Facture</b></td>    <td align='center'>%4</td>                     </tr>"
                "</table>";
        QString html1 =
                "<table border=\"0\">"

                "<tr > <td bgcolor=\"#FFCC00\" align='center'><b>Acte</b></td>   </tr>"
                "<tr> <td>%1</td>     </tr>"


                "<tr> <td bgcolor=\"#FFCC00\" align='center'><b>Unité</b></td>       </tr>"
                "<tr> <td align='center'>%2</td>    </tr>"

                "<tr> <td bgcolor=\"#FFCC00\" align='center'><b>MEO</b></td>    </tr>"
                "<tr> <td align='center'>%3</td>    </tr>"

                "<tr> <td bgcolor=\"#FFCC00\" align='center'><b>Fournitures</b></td>  </tr>"
                "<tr> <td align='center'>%4</td>    </tr>"

                "<tr> <td bgcolor=\"#FFCC03\" align='center'><b>Catégorie</b></td>  </tr>"
                "<tr> <td align='center'><font color=red>%5</font></td>    </tr>"

                "</table>";
        if(role==Qt::ToolTipRole)
        {




            if( this->tableName().contains("INCIDENTS"))

                return html.arg(QSqlTableModel::data( this->index( index.row(), 1),Qt::DisplayRole).toString())
                           .arg(QSqlTableModel::data( this->index( index.row(), 4),Qt::DisplayRole).toString())
                           .arg(QSqlTableModel::data( this->index( index.row(),5),Qt::DisplayRole).toString())
                           .arg(QSqlTableModel::data( this->index( index.row(), 6),Qt::DisplayRole).toString());

            else if (this->tableName().contains("Actes"))return html1.arg(QSqlTableModel::data( this->index( index.row(), 0),Qt::DisplayRole).toString())
                             .arg(QSqlTableModel::data( this->index( index.row(), 1),Qt::DisplayRole).toString())
                             .arg(QSqlTableModel::data( this->index( index.row(),2),Qt::DisplayRole).toString())
                             .arg(QSqlTableModel::data( this->index( index.row(), 3),Qt::DisplayRole).toString())
                             .arg(QSqlTableModel::data( this->index( index.row(), 4),Qt::DisplayRole).toString())
                             .arg(QSqlTableModel::data( this->index( index.row(), 5),Qt::DisplayRole).toString());



        }

        if (IMAGE_MODEL == 1){
            if(index.column()==2 && tableName().contains("Ressources_Humaines")  )
            {
                QString imgFile = (QString) QSqlTableModel::data(index,Qt::DisplayRole).toString();
                if (imgFile.size()<=0) imgFile.append(":/files/images/dummy_person.png");
                else imgFile = QCoreApplication::applicationDirPath().append(imgFile);
                if(role == Qt::DecorationRole ){
                    return QPixmap(imgFile).scaled(QSize(64,64),Qt::KeepAspectRatio);
                }
                else if (role == Qt::DisplayRole){
                    return "";
                }
            }
            if(index.column()==6 && tableName().contains("Stock_Entreprise")  )
            {
                QString imgFile = (QString) QSqlTableModel::data(index,Qt::DisplayRole).toString();
                if (imgFile.size()<=0) imgFile.append(":/files/images/dummy_person.png");
                else imgFile = QCoreApplication::applicationDirPath().append(imgFile);
                if(role == Qt::DecorationRole ){
                    return QPixmap(imgFile).scaled(QSize(64,64),Qt::KeepAspectRatio);
                }
                else if (role == Qt::DisplayRole){
                    return "";
                }
            }

        }
        else if (IMAGE_MODEL == 0) {

            if(this->tableName().contains("INCIDENTS")) {
                if(role == Qt::ForegroundRole ){
                    if(index.column()==5 || index.column()==6 || index.column()==7 ||index.column()==8 )
                        return QColor("green");
                    if(index.column()==14) return QColor("yellow");
                    if(index.column()==13) return QColor("red");

                }
                //                if(role == Qt::BackgroundRole )
                //                    if(index.column()==3 || index.column()==4 || index.column()==5 ||index.column()==6 )
                //                        return QColor("yellow");
                return QSqlTableModel::data(index,role);
            }

            return QSqlTableModel::data(index,role);
        }

        return QSqlTableModel::data(index,role);
    }

};

#endif // FORM_INCIDENT_H


