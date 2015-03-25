#include "configdialog.h"
#include "ui_configdialog.h"
#include<QStringListModel>
#include<qpushbutton.h>
#include"form_incident.h"
#include"mainwindow.h"

configDialog::configDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configDialog)
{
    MainWindow *mw=(MainWindow*)parent;
    ui->setupUi(this);

    QStringList list("Général");
    list.append("Incident");
    list.append("Travaux Neufs");
    list.append("Ressources Humaines");
    list.append("Entreprise");
    list.append("Localisation");

    list.append("Entretien");



    QStringListModel *model = new QStringListModel(list);
    ui->listView->setModel(model);
    ui->label->setText(ui->listView->model()->data(ui->listView->model()->index(0,0)).toString());
    QPushButton * buttonSave=(QPushButton *) ui->buttonBox->button(QDialogButtonBox::Save);
    buttonSave->setText("Sauver");
    buttonSave=(QPushButton *) ui->buttonBox->button(QDialogButtonBox::Cancel);
    buttonSave->setText("Annuler");
    EmployerModel *all_model = new EmployerModel(this, mw->getManager()->getDatabase(),1);
    all_model->setTable("Ressources_Humaines");
    all_model->select();
    ui->tableView->setModel(all_model);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->dateEdit->setDate(QDate::currentDate());
    QStringList feriers,gpsDevices;
    ui->listView_2->setModel(new QStringListModel(feriers));
    ui->listView_3->setModel(new QStringListModel(gpsDevices));


}

configDialog::~configDialog()
{
    delete ui;
}

void configDialog::on_listView_clicked(const QModelIndex &index)
{
    ui->label->setText(ui->listView->model()->data(index).toString());
    ui->stackedWidget->setCurrentIndex(index.row());
    ((QSqlTableModel*)ui->tableView->model())->select();

}

void configDialog::on_toolButton_clicked()
{
    ui->tableView->setEditTriggers( ui->tableView->editTriggers()!=0?QAbstractItemView::NoEditTriggers:QAbstractItemView::DoubleClicked);
    ui->toolButton->setIcon(ui->spinBox->isEnabled()? QIcon(":/files/images/closed_slot.png"):QIcon(":/files/images/open_slot.png"));
    ui->spinBox_pv_entretien->setEnabled(ui->spinBox_pv_entretien->isEnabled()?false:true);

    ui->spinBox->setEnabled(ui->spinBox->isEnabled()?false:true);
    ui->spinBox_2->setEnabled(ui->spinBox_2->isEnabled()?false:true);
    ui->spinBox_3->setEnabled(ui->spinBox_3->isEnabled()?false:true);
    ui->spinBox_4->setEnabled(ui->spinBox_4->isEnabled()?false:true);
    ui->dateEdit->setEnabled(ui->dateEdit->isEnabled()?false:true);
    ui->spinBox_5->setEnabled(ui->spinBox_5->isEnabled()?false:true);
    ui->toolButton_3->setEnabled(ui->toolButton_3->isEnabled()?false:true);

    ui->toolButton_4->setEnabled(ui->toolButton_4->isEnabled()?false:true);
    ui->toolButton_5->setEnabled(ui->toolButton_5->isEnabled()?false:true);
    ui->timeEdit->setEnabled(ui->timeEdit->isEnabled()?false:true);
    ui->timeEdit_2->setEnabled(ui->timeEdit_2->isEnabled()?false:true);

    ui->lineEdit->setEnabled(ui->lineEdit->isEnabled() ? false:true);

    ui->lineEdit_2->setEnabled(ui->lineEdit_2->isEnabled() ? false:true);

    ui->lineEdit_3->setEnabled(ui->lineEdit_3->isEnabled() ? false:true);

    ui->lineEdit_4->setEnabled(ui->lineEdit_4->isEnabled() ? false:true);

    ui->lineEdit_5->setEnabled(ui->lineEdit_5->isEnabled() ? false:true);
    ui->lineEdit_6->setEnabled(ui->lineEdit_6->isEnabled() ? false:true);

    ui->lineEdit_7->setEnabled(ui->lineEdit_7->isEnabled() ? false:true);

    ui->toolButton_6->setEnabled(ui->toolButton_6->isEnabled()?false:true);
    ui->toolButton_7->setEnabled(ui->toolButton_7->isEnabled()?false:true);
    ui->toolButton_8->setEnabled(ui->toolButton_8->isEnabled()?false:true);
    ui->toolButton_9->setEnabled(ui->toolButton_9->isEnabled()?false:true);
    ui->toolButton_10->setEnabled(ui->toolButton_10->isEnabled()?false:true);
    ui->lineEdit_8->setEnabled(ui->lineEdit_8->isEnabled() ? false:true);
    ui->spinBox_entretien_cv->setEnabled(ui->spinBox_entretien_cv->isEnabled()?false:true);
    ui->spinBox_entretien_cv_year->setEnabled(ui->spinBox_entretien_cv_year->isEnabled()?false:true);


}

void configDialog::on_toolButton_2_clicked()
{
    switch (ui->listView->currentIndex().row()) {
    case 0:
        ui->label_3->setText("Non disponible.");
        break;
    case 1:
        ui->label_3->setText("Clicker <b>Sauver</b>.");
        break;
    case 2:
        ui->label_3->setText("Non disponible.");
        break;
    case 3:
        ui->label_3->setText("Sauvegarde automatique.");
        break;
    default:
        break;
    }
}

void configDialog::on_toolButton_3_clicked()
{
    ui->tableView->model()->insertRow(0) ;
}

void configDialog::on_toolButton_4_clicked()
{
    QStringListModel *model=(QStringListModel *)ui->listView_2->model();
    QStringList lst= model->stringList();
  lst.append(ui->dateEdit->date().toString("dd/MM/yyyy"));
  model->setStringList(lst);
model->submit();
}

void configDialog::on_toolButton_5_clicked()
{if(!ui->listView_2->selectionModel()->selectedRows().isEmpty()){
        QStringListModel *model=(QStringListModel *)ui->listView_2->model();
        QStringList lst= model->stringList();
        lst.removeAt(ui->listView_2->selectionModel()->selectedRows().at(0).row());
        model->setStringList(lst);
        model->submit();
    }
}

void configDialog::on_toolButton_6_clicked()
{
    QString folder= QFileDialog::getExistingDirectory(this);
    ui->lineEdit_7->setText(folder);
}

void configDialog::on_toolButton_8_clicked()
{
    QString imgUrl= QFileDialog::getOpenFileName(this);
    ui->lineEdit_6->setText(imgUrl);
}

void configDialog::on_toolButton_7_clicked()
{
   if(!ui->tableView->selectionModel()->selectedRows().isEmpty())  ui->tableView->model()->removeRow(ui->tableView->selectionModel()->selectedRows().at(0).row());


}

void configDialog::on_toolButton_10_clicked()
{
    QStringListModel *model=(QStringListModel *)ui->listView_3->model();
    QStringList lst= model->stringList();
  lst.append(ui->lineEdit_8->text());
  model->setStringList(lst);
model->submit();
}

void configDialog::on_toolButton_9_clicked()
{
    if(!ui->listView_3->selectionModel()->selectedRows().isEmpty()){
            QStringListModel *model=(QStringListModel *)ui->listView_3->model();
            QStringList lst= model->stringList();
            lst.removeAt(ui->listView_3->selectionModel()->selectedRows().at(0).row());
            model->setStringList(lst);
            model->submit();
        }
}
