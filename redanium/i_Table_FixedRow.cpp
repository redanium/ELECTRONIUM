#include "i_table_fixedrow.h"
#include <QSoundEffect>
#include "ui_i_table_fixedrow.h"
#include"QStandardItemModel"
#include<QDebug>
#include"form_incident.h"
#include"incident.h"
#include"Incident_Travaux.h"
i_table_fixedrow::i_table_fixedrow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::i_table_fixedrow)
{
    ui->setupUi(this);
    QStandardItemModel *model1= new QStandardItemModel(0,5,this);
    model1->setObjectName("modelF");
    model1->setHeaderData(0, Qt::Horizontal, QObject::tr("Désignation"));
    model1->setHeaderData(1, Qt::Horizontal, QObject::tr("Quantité"));
    model1->setHeaderData(2, Qt::Horizontal, QObject::tr("Unité"));
    model1->setHeaderData(3, Qt::Horizontal, QObject::tr("P.U en H.T"));
    model1->setHeaderData(4, Qt::Horizontal, QObject::tr("Montant"));
    model1->setHeaderData(5, Qt::Horizontal, QObject::tr("HT"));

    ui->tableView_Fournitures->setModel(model1);
    ui->tableView_Fournitures->setAlternatingRowColors(true);
    ui->tableView_Fournitures->resizeColumnsToContents();
    ui->tableView_Fournitures->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_Fournitures->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    QStandardItemModel *model= new QStandardItemModel(0,5,this);
    model->setObjectName("modelM");

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Désignation"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Quantité"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Unité"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("P.U en H.T"));
    model->setHeaderData(4, Qt::Horizontal, QObject::tr("Montant"));
    model->setHeaderData(5, Qt::Horizontal, QObject::tr("HT"));
    ui->tableView_MEO->setModel(model);
    ui->tableView_MEO->setAlternatingRowColors(true);
    ui->tableView_MEO->resizeColumnsToContents();
    ui->tableView_MEO->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableView_MEO->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);



    QStandardItemModel *model2= new QStandardItemModel(5,1,this);

    model2->setHeaderData(0, Qt::Vertical, QObject::tr("TOTAL H.T"));
    model2->setHeaderData(1, Qt::Vertical, QObject::tr("R.G 5%"));
    model2->setHeaderData(2, Qt::Vertical, QObject::tr("TOTAL APRÈS R.G"));

    model2->setHeaderData(3, Qt::Vertical, QObject::tr("TOTAL 17%"));
    model2->setHeaderData(4, Qt::Vertical, QObject::tr("TOTAL T.T.C"));

    ui->tableView_Montant->setModel(model2);
    ui->tableView_Montant->setAlternatingRowColors(true);
    ui->tableView_Montant->resizeColumnsToContents();
    ui->tableView_Montant->horizontalHeader()->hide();
    ui->tableView_Montant->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_Montant->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QPalette pal=ui->lcdNumber->palette();
    pal.setColor(QPalette::Normal,QPalette::WindowText,QColor(0, 170, 255));
    ui->lcdNumber->setPalette(pal);
    QPalette pal2= ui->lcdNumber_2->palette();
    pal2.setColor(QPalette::Normal,QPalette::WindowText,QColor(0, 170, 255));
    ui->lcdNumber_2->setPalette(pal2);

}

i_table_fixedrow::~i_table_fixedrow()
{
    delete ui;
}

void i_table_fixedrow::on_label_2_linkActivated(const QString &link)
{
}

void i_table_fixedrow::on_toolButton_Meo_clicked()
{
    if(ui->tableView_MEO->isVisible()){

        ui->tableView_MEO->setVisible(false);
        ui->toolButton_Meo->setIcon(QIcon(":/files/images/kuba_arrow_button_set_4.png"));
    }
    else{
        ui->tableView_MEO->setVisible(true);
        ui->toolButton_Meo->setIcon(QIcon(":/files/images/kuba_arrow_button_set_1.png"));
    }
}

void i_table_fixedrow::on_toolButtonFournitures_clicked()
{
    if(ui->tableView_Fournitures->isVisible())
    {

        ui->tableView_Fournitures->setVisible(false);

        ui->toolButtonFournitures->setIcon(QIcon(":/files/images/kuba_arrow_button_set_4.png"));

    }

    else
    {
        ui->tableView_Fournitures->setVisible(true);
        ui->toolButtonFournitures->setIcon(QIcon(":/files/images/kuba_arrow_button_set_1.png"));
    }
}

void i_table_fixedrow::on_toolButton_clicked()
{

    Incident_Travaux *travaux=incident->getI_Travaux();
    QStandardItemModel* model= (QStandardItemModel*) ui->tableView_Fournitures->model();

    if(!ui->tableView_Fournitures->selectionModel()->selectedIndexes().isEmpty()){

        for(int i=0;i<ui->tableView_Fournitures->selectionModel()->selectedRows().size();i++)
        {
            travaux->deleteAt(ui->tableView_Fournitures->selectionModel()->selectedRows().at(i).row()+1,2);
            model->removeRow(ui->tableView_Fournitures->selectionModel()->selectedRows().at(i).row());
        }
    }

    model->submit();
    qDebug()<<  (incident==0)<< (incident->getI_Travaux()==0);
    incident->i_print();
    ui->lcdNumber->display(QString::number(model->rowCount()));
    updateTotal(0);
    QSoundEffect *sound = new QSoundEffect;
    sound->setSource(QUrl("qrc:/files/data/bin.wav"));
    sound->play();
    ((Form_incident*)incident->parent())->updateMoney();
    ((Form_incident*)incident->parent())->updatePreview(0);


}

void i_table_fixedrow::on_toolButton_2_clicked()
{


    Incident_Travaux *travaux=incident->getI_Travaux();
    QStandardItemModel* model= (   QStandardItemModel*) ui->tableView_MEO->model();

    if(!ui->tableView_MEO->selectionModel()->selectedIndexes().isEmpty()){

        for(int i=0;i<ui->tableView_MEO->selectionModel()->selectedRows().size();i++)
        {
            travaux->deleteAt(ui->tableView_MEO->selectionModel()->selectedRows().at(i).row()+1,1);

            model->removeRow(ui->tableView_MEO->selectionModel()->selectedRows().at(i).row());
        }
    }

    model->submit();
    qDebug()<<  (incident==0)<< (incident->getI_Travaux()==0);
    incident->i_print();
    ui->lcdNumber_2->display(QString::number(model->rowCount()));
updateTotal(1);
    QSoundEffect *sound = new QSoundEffect;
    sound->setSource(QUrl("qrc:/files/data/bin.wav"));
    sound->play();
    ((Form_incident*)incident->parent())->updateMoney();
    ((Form_incident*)incident->parent())->updatePreview(0);


}
Incident_obj *i_table_fixedrow::getIncident() const
{
    return incident;
}

void i_table_fixedrow::setIncident(Incident_obj *value)
{
    incident = value;
}

void i_table_fixedrow::updateTotal(int table)
{
    double total=0;
    QAbstractItemModel *model;
    switch (table) {
    case 0:
        model=ui->tableView_Fournitures->model();
        for (int i = 0; i < model->rowCount(); ++i) {
            total=total+model->data(model->index(i,4),Qt::DisplayRole).toDouble();
        }
        ui->label_total_fournitures_num->setText(QString::number(total));
        break;

    case 1:
        model=ui->tableView_MEO->model();
        total=0;
        for (int i = 0; i < model->rowCount(); ++i) {
            total=total+model->data(model->index(i,4),Qt::DisplayRole).toDouble();
        }
        ui->label_total_meo_num->setText(QString::number(total));
        break;
    default:
        break;
    }
}

