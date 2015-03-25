#include "Incidents_Form.h"
#include "ui_incidentsform.h"
#include<qstandarditemmodel.h>
#include"mainwindow.h"
#include"form_incident.h"
#include<QAction>
#include<QDialog>
#include<QDialogButtonBox>
#include<QSoundEffect>

Incidents_Form::Incidents_Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Incidents_Form)
{

    QSettings settings("Redanium", "Oso Manager");

    MainWindow *window;
    window=(MainWindow*)parent;
    ui->setupUi(this);

    EmployerModel *model1= new EmployerModel(this, window->getManager()->getDatabase());

    if(!window->getManager()->getDbTables().contains("INCIDENTS"))
        window->getManager()->create_Incident_Table();
    model1->setTable("INCIDENTS");
    ui->tableView->setModel(model1);
    model1->select();
    ui->tableView->setAlternatingRowColors(true);
    //ui->tableView->resizeColumnsToContents();

    ui->tableView->setColumnHidden(12,true);

    ui->toolButton_9->setAutoRepeat(true);
    emit ui->toolButton_9->clicked();

    QStringList headersList ;
    for (int i = 0; i < 16; ++i) {
        headersList<< model1->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()  ;
    }
    QStringListModel *listModel= new QStringListModel;
    listModel->setStringList(headersList);
    ui->comboBox->setModel(listModel);
    ui->label_3->setText(QString::number(model1->rowCount()));

    for (int i = 0; i < 16; ++i) {

        if(!settings.value(QString("incidents_list/%1").arg(ui->tableView->model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString())).toBool())
        {
            ui->tableView->hideColumn(i);



        }
        else {

            ui->tableView->showColumn(i);

        }
    }
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->hide();
    // ui->tableView->setAlternatingRowColors(true);
    ui->tableView->horizontalHeader()->setSectionsMovable(true);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    updatePlot();

}

Incidents_Form::~Incidents_Form()
{
    delete ui;
}

void Incidents_Form::updatePlot()
{
    QStringList labels,tmplstlb,tmplstdt;
    QList<QDate>maplist;

    for(int i=0;i<ui->tableView->model()->rowCount();i++){
        labels.append(ui->tableView->model()->data(ui->tableView->model()->index(i,4),Qt::DisplayRole).toString());
        maplist.append(QDate::fromString(ui->tableView->model()->data(ui->tableView->model()->index(i,4),Qt::DisplayRole).toString(),"d/M/yyyy"));

    }
    // qDebug()<< labels.join(",")<< "/n"<<datas.join(",");
    QFile filehtml;
    //open the html template from ressources
    filehtml.setFileName(":/files/data/graphs.html");

    qSort(maplist.begin(),maplist.end());
    qDebug()<<maplist;
    int i=0;
    while(i<ui->tableView->model()->rowCount())

    {
        for(int j=0;j<ui->tableView->model()->rowCount();j++)

        {
            if(maplist[i]==QDate::fromString(labels[j],"d/M/yyyy"))
            {

                tmplstlb.append(labels[j]);
                tmplstdt.append(ui->tableView->model()->data(ui->tableView->model()->index(j,13),Qt::DisplayRole).toString());
                tmplstlb[i].prepend("\"");
                tmplstlb[i].append("\"");
                tmplstdt[i].prepend("\"");
                tmplstdt[i].append("\"");
                labels[j]="000000";
                break;

            }


        }
        i++;
    }
    maplist.clear();
    qDebug()<<tmplstdt<<tmplstlb;
    if (filehtml.open(QFile::ReadOnly | QFile::Text))
        qDebug()<<"graphs.html is open : "<<filehtml.isOpen();
    QTextStream in(&filehtml);
    QString str = QString(in.readAll());

    ui->webView->setHtml(str.arg(tmplstlb.join(",")).arg(tmplstdt.join(",")));
    qDebug()<<ui->webView->page()->mainFrame()->toHtml();
}





void Incidents_Form::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    //    EmployerModel * myModel= (EmployerModel *)ui->tableView->model();
    //    QString str=myModel->filter();
    //    str.append(arg1);
    //    str.append("'");
    //    myModel->setFilter(arg1);

}

void Incidents_Form::on_comboBox_currentIndexChanged(const QString &arg1)
{
    /*
    EmployerModel * myModel=(EmployerModel *)ui->tableView->model();
    myModel->setFilter(arg1);
*/
}

void Incidents_Form::on_lineEdit_editingFinished()
{
    EmployerModel * myModel = (EmployerModel *) ui->tableView->model();
    QString str;
    str.append(ui->comboBox->currentText());
    if(ui->comboBox_2->currentText().contains("contient"))
    {
        str.append(" LIKE ");
        str.append(" '%");
        str.append(ui->lineEdit->text());
        str.append("%' ");
        myModel->setFilter(str);
    }
    else {
        str.append(ui->comboBox_2->currentText());
        str.append("'");
        str.append(ui->lineEdit->text());
        str.append("'");
        myModel->setFilter(str);
    }
    // ui->label_4->setText(myModel->filter());
}

void Incidents_Form::on_toolButton_3_clicked()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if(!ui->tableView->selectionModel()->selectedIndexes().isEmpty()){

        for(int i=0;i<ui->tableView->selectionModel()->selectedRows().size();i++)
        {     ui->tableView->model()->removeRow(ui->tableView->selectionModel()->selectedRows().at(i).row());}


        qDebug()<< "selected rows" << ui->tableView->selectionModel()->selectedRows().size();
        EmployerModel* model= (  EmployerModel*) ui->tableView->model();
        model->select();
        ui->label_5->setText(QString::number( ui->tableView->selectionModel()->selectedRows().size()));

    }
    QSoundEffect *sound = new QSoundEffect;
    sound->setSource(QUrl("qrc:/files/data/bin.wav"));
    sound->play();
    QApplication::restoreOverrideCursor();


}

void Incidents_Form::on_toolButton_4_clicked()
{
    if(ui->stackedWidget->currentIndex()==0){



        //ui->stackedWidget->removeWidget();
        //         ui->verticalLayout_5->insertWidget(0,);
        ui->stackedWidget_2->setCurrentIndex(1);
        ui->toolButton_4->setIcon(QIcon(":/files/images/table.png"));
        ui->stackedWidget->setCurrentIndex(1);

        updatePlot();
    }
    else {
        ui->toolButton_4->setIcon(QIcon(":/files/images/statistics.png"));

        ui->stackedWidget->setCurrentIndex(0);
        ui->stackedWidget_2->setCurrentIndex(0);



    }
}

void Incidents_Form::on_toolButton_5_clicked()
{   if(ui->tableView->editTriggers()==0)

    { ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->toolButton_5->setIcon(QIcon(":/files/images/open_slot.png"));

    }

    else    {
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->toolButton_5->setIcon(QIcon(":/files/images/closed_slot.png"));

    }

}

void Incidents_Form::on_toolButton_clicked()
{
    EmployerModel * myModel = (EmployerModel *) ui->tableView->model();
    QString str;
    str.append(ui->comboBox->currentText());
    if(ui->comboBox_2->currentText().contains("contient"))
    {
        str.append(" LIKE ");
        str.append(" '%");
        str.append(ui->lineEdit->text());
        str.append("%' ");
        myModel->setFilter(str);
    }
    else {
        str.append(ui->comboBox_2->currentText());
        str.append("'");
        str.append(ui->lineEdit->text());
        str.append("'");
        myModel->setFilter(str);
    }
}

void Incidents_Form::on_tableView_clicked(const QModelIndex &index)
{
    ui->label_5->setText(QString::number( ui->tableView->selectionModel()->selectedRows().size()));
}

void Incidents_Form::on_toolButton_2_clicked()
{
    ui->tableView->model()->insertRow(ui->tableView->model()->rowCount());
    ui->tableView->scrollToBottom();
}

void Incidents_Form::on_toolButton_7_clicked()
{
    EmployerModel* model= (  EmployerModel*) ui->tableView->model();
    if(!ui->tableView->selectionModel()->selectedIndexes().isEmpty()){

        for(int j=0;j<ui->tableView->selectionModel()->selectedRows().size();j++) {


            model->insertRow(model->rowCount());

            for(int i=1;i<16;i++){
                qDebug()<<model->data(model->index(ui->tableView->selectionModel()->selectedRows().at(j).row(),i),Qt::DisplayRole).toString();
                model->setData( model->index(model->rowCount()-1,i),
                                model->data(model->index(ui->tableView->selectionModel()->selectedRows().at(j).row(),i),Qt::DisplayRole).toString(),Qt::EditRole);
            }
            model->submit();
            //model->select();
        }
    }

    model->select();
}



void Incidents_Form::on_toolButton_6_clicked()
{
    Incident_obj *incident = new Incident_obj(this);

    EmployerModel* model= (  EmployerModel*) ui->tableView->model();


    for(int i=1;i<16;i++){

        //  qDebug()<<model->data(model->index(ui->tableView->selectionModel()->selectedRows().at(j).row(),i),Qt::DisplayRole).toString();
        //                   model->setData( model->index(model->rowCount()-1,i),model->data(model->index(ui->tableView->selectionModel()->selectedRows().at(j).row(),i),Qt::DisplayRole).toString(),Qt::EditRole);
    }



}

void Incidents_Form::on_toolButton_8_clicked()
{
    QSettings settings("Redanium", "Oso Manager");
    QVBoxLayout *layout = new QVBoxLayout;
    colDlg=new QDialog(this,Qt::Popup);
    colDlg->setWindowTitle("Visibilité de colones");
    colDlg->setLayout(layout);

    QCheckBox *box;
    QDialogButtonBox *buttons = new QDialogButtonBox(QDialogButtonBox::Ok| QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(accepted()), colDlg, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), colDlg, SLOT(reject()));
    if(settings.allKeys().contains("incidents_list/Objet")){

        for (int i = 0; i < ui->tableView->model()->columnCount(); ++i) {
            layout->insertWidget(i,new QCheckBox(ui->tableView->model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()));
            box=  (QCheckBox*) layout->itemAt(i)->widget();
            box->setCheckable(true);
            box->setChecked(settings.value(QString("incidents_list/%1").arg(box->text())).toBool());

        }
    }
    else {


        for (int i = 0; i < ui->tableView->model()->columnCount(); ++i) {
            layout->insertWidget(i,new QCheckBox(ui->tableView->model()->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()));
            box=  (QCheckBox*) layout->itemAt(i)->widget();
            box->setCheckable(true);
            box->setChecked(true);

        }




    }
    layout->insertWidget(16,buttons);

    if(colDlg->exec()==QDialog::Accepted)
    {

        for (int i = 0; i < 16; ++i) {

            QCheckBox *box=  (QCheckBox*) layout->itemAt(i)->widget();
            if(!box->isChecked())
            {
                ui->tableView->hideColumn(i);
                settings.setValue(QString("incidents_list/%1").arg(box->text()),box->isChecked());



            }
            else {

                settings.setValue(QString("incidents_list/%1").arg(box->text()),box->isChecked());
                ui->tableView->showColumn(i);

            }
        }




    }
    //    this->myMenu= new QMenu("Colones",this);
    //    QList<QAction*> actions;
    //    for (int i = 0; i < ui->tableView->model()->columnCount(); ++i) {
    //       QAction *act=new QAction(ui->tableView->model()->headerData(i,Qt::Horizontal,Qt::DisplayRole),this);
    //        actions.append(act);
    //       act->setCheckable(true);
    //    }
    //    myMenu->addActions(actions);
    //    for (int i = 0; i < ui->tableView->model()->columnCount(); ++i) {
    //connect(this,SIGNAL(connectMenu(int)),ui->tableView,SLOT(hideColumn(int)));
    //    }
    //    myMenu->show();
}
void Incidents_Form::connectMenu(int i)
{

    //this->myMenu->actionAt(i)->trigger();


}

void Incidents_Form::on_toolButton_9_clicked()
{
    EmployerModel* model= (  EmployerModel*) ui->tableView->model();
    model->select();
    updatePlot();
}

void Incidents_Form::on_toolButton_open_clicked()
{
    EmployerModel* model= (  EmployerModel*) ui->tableView->model();
    MainWindow *window;
    window=(MainWindow*)this->parent();
    foreach (QModelIndex indx,ui->tableView->selectionModel()->selectedRows()) {




        Incident_obj* incident=new Incident_obj();
        incident->set_i_NumID(model->data(model->index(indx.row(),0),Qt::DisplayRole).toDouble());
        incident->set_i_Objet(model->data(model->index(indx.row(),1),Qt::DisplayRole).toString());
        incident->setI_Direction(model->data(model->index(indx.row(),2),Qt::DisplayRole).toString());
        Form_incident *incident_ui = new Form_incident(window);
        // incident_ui->setIncident(incident);
        incident_ui->findChild<QLCDNumber*>("lcdNumber")->display(QString::number(incident_ui->getIncident()->getI_Travaux()->getFournitures().size()));
        incident_ui->findChild<QLCDNumber*>("lcdNumber_2")->display(QString::number(incident_ui->getIncident()->getI_Travaux()->getMEO().size()));


        window->findChild<QMdiArea*>("mdiArea")->addSubWindow(incident_ui);
        incident_ui->showMaximized();


    }

}
