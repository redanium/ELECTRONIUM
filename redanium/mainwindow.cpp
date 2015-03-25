#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QLocale>
#include<QAxWidget>
#include"incident.h"
#include<QCalendarWidget>
#include<QDialog>
#include"DatabaseManager.h"
#include<QDebug>
#include<QStringListModel>
#include<QTableWidgetItem>
#include<QSqlTableModel>
#include<QStandardItemModel>
#include<QToolButton>
#include"Incident_Toolbar.h"
#include"Form_Incident.h"
#include<QPushButton>
#include<QFileDialog>
#include<QPrintPreviewDialog>
#include"Incidents_Form.h"
#include"configdialog.h"
#include<QNetworkAccessManager>
#include<QNetworkInterface>
#include"QProgressIndicator.h"
#include<QSoundEffect>
#include<qwebframe.h>
#include<QPrintDialog>
#include<qabstractitemmodel.h>


#ifndef Q_OS_ANDROID
#include<QtWebKitWidgets/QWebView>
#endif



Incidents_Form* MainWindow::Incidents_Form_List =0;

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent),ui(new Ui::MainWindow){
    ui->setupUi(this);
    ui->tableView_Travaux->setEnabled(false);
    // init the DB manager and open DB
    manager=new DatabaseManager();
    manager->openDB();
    //Preview init
    webview =new QWebView;
    webview->setUrl(QUrl("qrc:/files/data/template.html"));
    webview->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls,true);
    webview->settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls,true);
    webview->settings()->setAttribute(QWebSettings::LocalStorageEnabled,true);
    webview->settings()->setAttribute(QWebSettings::AutoLoadImages,true);
    webview->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled,true);
    webview->settings()->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled,true);
    webview->settings()->setAttribute(QWebSettings::PrintElementBackgrounds,true);


    windowMapper = new QSignalMapper(this);
    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(activateSubWindow(QWidget*)));
    //#####################################################################################################
    //############################# Creates a toolbar and connect the slots & signals #####################
    //#####################################################################################################
    addToolBar(new Incident_Toolbar(this,0) );
    addToolBar(new Incident_Toolbar(this,1) );
    addToolBar(new Incident_Toolbar(this,2) );

    toolbars.append(findChild<QToolBar*>("toolBar_0"));
    toolbars.append(findChild<QToolBar*>("toolBar_1"));
    toolbars.append(findChild<QToolBar*>("toolBar_2"));



    QAction *stats=toolbars.at(0)->actions().at(0);
    connect(stats,SIGNAL(triggered()),ui->actionIncident,SLOT(trigger()));

    stats=toolbars.at(0)->actions().at(2);
    connect(stats,SIGNAL(triggered()),ui->actionEntretien,SLOT(trigger()));


    stats=toolbars.at(0)->actions().at(3);
    connect(stats,SIGNAL(triggered()),this,SLOT(showIncidentsTable()));

    stats=toolbars.at(0)->actions().at(4);
    connect(stats,SIGNAL(triggered()),this,SLOT(on_action_lock_trigered()));

    stats=toolbars.at(1)->actions().at(1);
    connect(stats,SIGNAL(triggered()),this,SLOT(filePrintPreview()));

    stats=toolbars.at(1)->actions().at(0);
    connect(stats,SIGNAL(triggered()),this,SLOT(on_actionSur_base_de_donn_es_triggered()));


    stats=toolbars.at(1)->actions().at(3);
    connect(stats,SIGNAL(triggered()),this,SLOT(on_actionImprimer_2_triggered()));


    connect(ui->actionImprimer_Pdf,SIGNAL(triggered()),this,SLOT(filePrintPDF()));
    stats=toolbars.at(1)->actions().at(2);
    connect(stats,SIGNAL(triggered()),this,SLOT(filePrintPDF()));
    toolbars.at(1)->setVisible(false);

    //setup model and view for "Travaux"
    ui->tableView_Travaux->setAlternatingRowColors(true);
    ui->tableView_Travaux->resizeColumnsToContents();
    ui->tableView_Travaux->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView_Travaux->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    //Dictionary for Objet
    QStringList outils =manager->getDbTables();
    outils.removeAt(4);
    outils.prepend("Listes Actes et ressources humaines");
    ui->comboBox->setModel(new QStringListModel(outils));
    ui->comboBox->setCurrentIndex(0);

    //check if there is a network connection (must replace with a ping to google)
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;

    for (int i = 0; i < ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if ( iface.flags().testFlag(QNetworkInterface::IsUp)
             && !iface.flags().testFlag(QNetworkInterface::IsLoopBack) )
        {    // this loop is important
            for (int j=0; j<iface.addressEntries().count(); j++)
            {

                // we have an interface that is up, and has an ip address
                // therefore the link is present

                // we will only enable this check on first positive,
                // all later results are incorrect
                if (result == false)
                    result = true;
            }
        }

    }
    if (!result){
        ui->webView->setUrl(QUrl("qrc:/files/data/maps_static.html"));
    }

    ui->actionIncident->trigger();

    // search field ////////
    //    QStringList headersList ;

    //    headersList<< "Acte"<< "Unité"<< "MEO"<< "Fourniture"<< "Catégorie";

    //    QStringListModel *listModel= new QStringListModel;
    //    listModel->setStringList(headersList);
    //    ui->comboBox_2->setModel(listModel);
    ui->tabWidget->tabBar()->setTabTextColor(0,"white");
    ui->tabWidget->tabBar()->setTabTextColor(1,"white");
    QToolButton *closePdfBtn= new QToolButton;//(ui->axWidget);
    closePdfBtn->setIcon(QIcon(":/files/images/close.png"));
    connect(closePdfBtn,SIGNAL(clicked()),this,SLOT(on_PdfBtn_clicked()));
    ui->webView->page()->mainFrame()->addToJavaScriptWindowObject("i_distance", ui->lineEdit);
    QSettings settings("Redanium", "Oso Manager");
    QStringList list1;
    for (int i = 0; i < settings.value(QString("localisation/gpsdevices_nbr")).toInt(); ++i) {
        list1.append( settings.value(QString("localisation/gpsdevice_%1").arg(i)).toString());

    }
    ui->listView->setModel(new QStringListModel(list1));

    QSqlTableModel *model1= new QSqlTableModel(this,manager->getDatabase());


    ui->tableView->setModel(model1);
    model1->setTable("gps");
    model1->select();
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::FindItemRow(QString item,QTableView *table)
{
    qDebug()<<"Find Item Row Number : "<< "Working ...";
    int row=table->model()->rowCount();
    qDebug()<<"Find Item Table Row count : "<< row;

    for (int i=0;i<row;i++)
    {
        if(table->model()->data(table->model()->index(i,0),Qt::DisplayRole).toString().trimmed()==item.trimmed())
        {qDebug()<<"Find Item Row Number : "<< "ok! "<<"Row Number " <<i;
            return i;
        }

    }
    qDebug()<<"Find Item Row Number : "<< "Failed !";
    return -1;

}
//should move it as a static fct to FORM_INCIDENT class
void MainWindow::UpdateMoney()
{
    Form_incident *currentFormIncident =(Form_incident*) ui->mdiArea->activeSubWindow()->widget();
    Incident_obj *incident=currentFormIncident->getIncident();
    double montant_temp =0;
    QTableView *tableViewMontant = currentFormIncident->findChild<QTableView*>("tableView_Montant");
    QStandardItemModel *model_montant = qobject_cast<QStandardItemModel*>(tableViewMontant->model());
    QTableView *ttable_meo = currentFormIncident->findChild<QTableView*>("tableView_MEO");

    qDebug()<<"model without image  : "<< "good till here -1";
    QTableView *ttable_F =currentFormIncident->findChild<QTableView*>("tableView_Fournitures");
    qDebug()<<"model without image  : "<< "good till here 0";
    QStandardItemModel   *modelMeo= (QStandardItemModel*) ttable_meo->model();
    QStandardItemModel   *modelF= (QStandardItemModel*) ttable_F->model();
    // modelF->append
    for(int i=0 ; i<modelF->rowCount() ; i++)
    {
        montant_temp += modelF->data(modelF->index(i,4),Qt::DisplayRole).toDouble();
        qDebug()<<"Montant : "<< montant_temp;
    }

    for(int i=0;i<modelMeo->rowCount();i++)
    {
        montant_temp += modelMeo->data(modelMeo->index(i,4),Qt::DisplayRole).toDouble();
        qDebug()<<"Montant : "<< montant_temp;
    }
    //    Q_UNUSED(modelF)
    //    Q_UNUSED(modelMeo)
    // model_montant->insertColumn(1);
    model_montant->setData(model_montant->index(0,0),QString::number(montant_temp),Qt::DisplayRole); // Total HT
    model_montant->setData(model_montant->index(1,0),QString::number(montant_temp*0.05),Qt::DisplayRole); // RG 5%
    model_montant->setData(model_montant->index(2,0),QString::number(montant_temp-montant_temp*0.05),Qt::DisplayRole); // Total apres RG 5%
    model_montant->setData(model_montant->index(3,0),(montant_temp-montant_temp*0.05)*0.17,Qt::DisplayRole); // Total 17%
    model_montant->setData(model_montant->index(4,0),(montant_temp-montant_temp*0.05)*1.17,Qt::DisplayRole); // TOTAL TTC
    QLabel *moneyTXT=currentFormIncident->findChild<QLabel*>("MoneyTextEdit");
    moneyTXT->setText(QString("<b><font color=\"green\">Arrêter la présente facture à la somme de :</fon></b>\n\n%1").arg("<b>"+Incident_obj::ChiffreEnLettre(montant_temp)+"</b>"));
    incident->set_i_Montant(montant_temp);
}

void MainWindow::filePrintPreview()
{
#ifdef Q_OS_ANDROID
    ui->actionApercu->setEnabled(false);
#endif
#ifndef Q_OS_ANDROID
    // display print preview dialog
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFileName("dummy");
    QPrintPreviewDialog preview(&printer,this,Qt::Window);
    preview.setWindowIcon(QIcon(":/files/images/print.png"));
    preview.setWindowTitle("Aprecu avant impression");
    // Print(printer,QPrinter::NativeFormat,"",true);

    if(ui->tabWidget_3->currentIndex()==0) currentActiveForm->getIncident()->i_Preview(0,ui->webView_2);
    else    if(ui->tabWidget_3->currentIndex()==1)     currentActiveForm->getIncident()->i_Preview(1,ui->webView_2);
    else  currentActiveForm->getIncident()->i_Preview(2,ui->webView_2);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)),ui->webView_2, SLOT(print(QPrinter*)));

    preview.setGeometry(0,40,QApplication::screens().at(0)->size().width()/2,QApplication::screens().at(0)->size().height()*2/3);

    preview.exec();
#endif
}

void MainWindow::filePrintPDF()
{
    Form_incident *currentFormIncident =(Form_incident*) ui->mdiArea->activeSubWindow()->widget();
    Incident_obj *incident=currentFormIncident->getIncident();
    currentActiveForm = currentFormIncident;
    QPrinter printer(QPrinter::HighResolution);
    //##################################
    QDialog *dialog= new QDialog(this);
    QVBoxLayout *boxLayout = new QVBoxLayout(dialog);
    QLabel *date_facture = new QLabel("Date de facture :");
    QDateEdit *datefac = new QDateEdit;
    datefac->setDisplayFormat("dd/MM/yyyy");
    datefac->setDate(QDate::currentDate());
    datefac->setCalendarPopup(true);
    QLabel *numero_facture = new QLabel("Numéro de facture :");
    QLineEdit *num_fac = new QLineEdit;
    boxLayout->addWidget(date_facture);
    boxLayout->addWidget(datefac);
    boxLayout->addWidget(numero_facture);
    boxLayout->addWidget(num_fac);
    QHBoxLayout *hl=new QHBoxLayout;
    QPushButton *okBtn = new QPushButton("Ok");
    okBtn->setAutoDefault(true);
    QPushButton *cancelBtn = new QPushButton("Annler");
    hl->addWidget(okBtn);
    hl->addWidget(cancelBtn);
    boxLayout->addLayout(hl);
    dialog->setLayout(boxLayout);
    dialog->setWindowTitle("Facture");
    QString fileName ;
    //##################################
    connect(okBtn,SIGNAL(clicked()),dialog,SLOT(accept()));
    connect(cancelBtn,SIGNAL(clicked()),dialog,SLOT(reject()));
    if(dialog->exec()==QDialog::Accepted) {
        incident->setI_Date_Facture(datefac->text());
        incident->setI_numero_facture(num_fac->text().toInt());

        printer.setOutputFormat(QPrinter::PdfFormat);
        fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                QString(), "*.pdf");
        if (!fileName.isEmpty())
            if (QFileInfo(fileName).suffix().isEmpty())
                fileName.append(".pdf");
        printer.setOutputFileName(fileName);
        EmployerModel * model = new EmployerModel(this,manager->getDatabase(),0);
        model->setTable("\"Factures\"");
        model->select();
        model->insertRow(0);
        model->setData(model->index(0,0),incident->getI_numero_facture());
        model->setData(model->index(0,1),incident->get_i_Convention());
        model->setData(model->index(0,2),incident->getType()==Form_incident::INCIDENT ? "x":" ");
        model->setData(model->index(0,3),incident->getType()==Form_incident::ENTRETIEN ? "x":" ");
        model->submit();

        if(ui->tabWidget_3->currentIndex()==0) currentActiveForm->getIncident()->i_Preview(0,ui->webView_2);
        else    if(ui->tabWidget_3->currentIndex()==1)     currentActiveForm->getIncident()->i_Preview(1,ui->webView_2);
        else  currentActiveForm->getIncident()->i_Preview(2,ui->webView_2);
        statusBar()->showMessage(fileName.append(Printp(&printer)==1?" enregistré avec succés":" non enregistré"));
        model->deleteLater();
    }

}

int MainWindow::Print(QPrinter *printer,QPrinter::OutputFormat outputFormat,QString filename,bool isPreview )
{

#ifdef Q_OS_ANDROID
    ui->actionImprimer_2->setEnabled(false);
#endif
#ifndef Q_OS_ANDROID
    //set the printer
    printer->setResolution(QPrinter::HighResolution);
    printer->setPageSize(QPrinter::A4);
    printer->setPaperSize(QPrinter::A4);
    printer->setOutputFormat(outputFormat);
    if(!filename.isEmpty()||!filename.isNull())
    {
        printer->setOutputFileName(filename);qDebug() <<filename ;
    }


    printer->setPageSize(QPrinter::A4);
    printer->setPageMargins(13,13,13,13,QPrinter::Millimeter);





    if(!filename.isEmpty()||!filename.isNull()){
        printer->setOutputFileName(filename);

    }
    if(!isPreview){
        QPrintDialog print_dlg(printer,this);
        if (print_dlg.exec() == QDialog::Accepted) {


            ui->webView_2->print(printer);

        }
    }


    return 1;
#endif
}

void MainWindow::updateQty()
{

}

int MainWindow::Printp(QPrinter *printer )
{


#ifndef Q_OS_ANDROID
    if(!printer->outputFileName().isEmpty()) {
        printer->setPageSize(QPrinter::A4);
        printer->setPageMargins(13,7,13,2,QPrinter::Millimeter);


        printer->setPrintRange(QPrinter::AllPages);


        if(ui->tabWidget_3->currentIndex()==0) currentActiveForm->getIncident()->i_Preview(0,ui->webView_2);
        else    if(ui->tabWidget_3->currentIndex()==1)     currentActiveForm->getIncident()->i_Preview(1,ui->webView_2);
        else  currentActiveForm->getIncident()->i_Preview(2,ui->webView_2);
        ui->webView_2->print(printer);



        return 1;
    }
    else return 0;
#endif
}

void MainWindow::showIncidentsTable()
{
static QMdiSubWindow *subWin;
    if(!Incidents_Form_List) {
        Incidents_Form_List= new Incidents_Form(this);

        Incidents_Form_List->setWindowTitle("Liste des incidents");
        subWin=ui->mdiArea->addSubWindow(Incidents_Form_List);
        ui->tableView_Travaux->setEnabled(false);
        Incidents_Form_List->showNormal();
        connect(Incidents_Form_List,SIGNAL(destroyed(QObject*)),this,SLOT(clean(QObject*)));


    }
   else ui->mdiArea->setActiveSubWindow(subWin);

}

void MainWindow::saveOptions()
{
    qDebug()<<"Saving Settings.";
    QSettings settings("Redanium", "Oso Manager");
    //entretien
    settings.setValue("entretien/point_value",   dialog->findChild<QSpinBox*>("spinBox_pv_entretien")->value());
    settings.setValue("entretien/convention_numero",   dialog->findChild<QSpinBox*>("spinBox_entretien_cv")->value());
    settings.setValue("entretien/convention_année",   dialog->findChild<QSpinBox*>("spinBox_entretien_cv_year")->value());
    //Incident
    settings.setValue("incident/point_value",   dialog->findChild<QSpinBox*>("spinBox")->value());
    settings.setValue("incident/alerte_convention",   dialog->findChild<QSpinBox*>("spinBox_2")->value());
    settings.setValue("incident/convention_numero",   dialog->findChild<QSpinBox*>("spinBox_5")->value());

    settings.setValue("incident/convention_année",   dialog->findChild<QSpinBox*>("spinBox_3")->value());
    settings.setValue("incident/convention_total",   dialog->findChild<QSpinBox*>("spinBox_4")->value());
    settings.setValue("incident/interval_maj",   dialog->findChild<QSpinBox*>("spinBox_6")->value());
    //GENERAL
    QListView *listFeriers = dialog->findChild<QListView*>("listView_2");
    settings.setValue(QString("general/jourFerier_nbr"),   listFeriers->model()->rowCount());
    for (int i = 0; i < listFeriers->model()->rowCount(); ++i) {
        settings.setValue(QString("general/jourFerier_%1").arg(i),   listFeriers->model()->data(listFeriers->model()->index(i,0)).toString());

    }

    settings.setValue(QString("general/travail_nuit_debut"),  dialog->findChild<QTimeEdit*>("timeEdit_2")->time().toString("hh:mm"));
    settings.setValue(QString("general/travail_nuit_fin"),  dialog->findChild<QTimeEdit*>("timeEdit")->time().toString("hh:mm"));
    //Entreprise
    settings.setValue(QString("entreprise/nom"),  dialog->findChild<QLineEdit*>("lineEdit")->text());
    settings.setValue(QString("entreprise/immatriculation"),  dialog->findChild<QLineEdit*>("lineEdit_2")->text());
    settings.setValue(QString("entreprise/article_imposition"),  dialog->findChild<QLineEdit*>("lineEdit_3")->text());
    settings.setValue(QString("entreprise/rc"),  dialog->findChild<QLineEdit*>("lineEdit_4")->text());
    settings.setValue(QString("entreprise/r_id_bank"),  dialog->findChild<QLineEdit*>("lineEdit_5")->text());
    settings.setValue(QString("entreprise/logo"),  dialog->findChild<QLineEdit*>("lineEdit_6")->text());

    //RH
    settings.setValue(QString("rh/dossier_photo"),  dialog->findChild<QLineEdit*>("lineEdit_7")->text());
    //localisation
    QListView *listgpsDevices = dialog->findChild<QListView*>("listView_3");
    settings.setValue(QString("localisation/gpsdevices_nbr"),   listgpsDevices->model()->rowCount());
    for (int i = 0; i < listgpsDevices->model()->rowCount(); ++i) {
        settings.setValue(QString("localisation/gpsdevice_%1").arg(i),   listgpsDevices->model()->data(listgpsDevices->model()->index(i,0)).toString());

    }

}

void MainWindow::readOptions()
{
    qDebug()<<"Reading Settings.";
    QSettings settings("Redanium", "Oso Manager");
    dialog->findChild<QSpinBox*>("spinBox_pv_entretien")->setValue( settings.value("entretien/point_value").toInt());
    dialog->findChild<QSpinBox*>("spinBox_entretien_cv_year")->setValue( settings.value("entretien/convention_année").toInt());
    dialog->findChild<QSpinBox*>("spinBox_entretien_cv")->setValue( settings.value("entretien/convention_numero").toInt());



    dialog->findChild<QSpinBox*>("spinBox")->setValue( settings.value("incident/point_value").toInt());
    dialog->findChild<QSpinBox*>("spinBox_2")->setValue( settings.value("incident/alerte_convention").toInt());
    dialog->findChild<QSpinBox*>("spinBox_3")->setValue( settings.value("incident/convention_année").toInt());
    dialog->findChild<QSpinBox*>("spinBox_4")->setValue( settings.value("incident/convention_total").toInt());
    dialog->findChild<QSpinBox*>("spinBox_5")->setValue( settings.value("incident/convention_numero").toInt());
    dialog->findChild<QSpinBox*>("spinBox_6")->setValue( settings.value("incident/interval_maj").toInt());
    QListView *listFeriers = dialog->findChild<QListView*>("listView_2");
    QStringList list;
    for (int i = 0; i < settings.value(QString("general/jourFerier_nbr")).toInt(); ++i) {
        list.append( settings.value(QString("general/jourFerier_%1").arg(i)).toString());

    }
    (( QStringListModel *)listFeriers->model())->setStringList(list);
    dialog->findChild<QTimeEdit*>("timeEdit_2")->setTime(QTime::fromString(settings.value("general/travail_nuit_debut").toString(),"hh:mm"));
    dialog->findChild<QTimeEdit*>("timeEdit")->setTime(QTime::fromString(settings.value("general/travail_nuit_fin").toString(),"hh:mm"));

    dialog->findChild<QLineEdit*>("lineEdit")->setText(   settings.value(QString("entreprise/nom")                 ).toString());
    dialog->findChild<QLineEdit*>("lineEdit_2")->setText( settings.value(QString("entreprise/immatriculation")     ).toString());
    dialog->findChild<QLineEdit*>("lineEdit_3")->setText( settings.value(QString("entreprise/article_imposition")  ).toString());
    dialog->findChild<QLineEdit*>("lineEdit_4")->setText( settings.value(QString("entreprise/rc" )                 ).toString());
    dialog->findChild<QLineEdit*>("lineEdit_6")->setText( settings.value(QString("entreprise/logo")           ).toString());
    dialog->findChild<QLineEdit*>("lineEdit_5")->setText( settings.value(QString("entreprise/r_id_bank" )                 ).toString());

    dialog->findChild<QLineEdit*>("lineEdit_7")->setText( settings.value(QString("rh/dossier_photo")           ).toString());

    QListView *listgpsDevices = dialog->findChild<QListView*>("listView_3");
    QStringList list1;
    for (int i = 0; i < settings.value(QString("localisation/gpsdevices_nbr")).toInt(); ++i) {
        list1.append( settings.value(QString("localisation/gpsdevice_%1").arg(i)).toString());

    }
    (( QStringListModel *)listgpsDevices->model())->setStringList(list1);

}

void MainWindow::draw_graph_tab()
{
    //    ui->tabWidget->addTab(    Incident_obj::i_Draw_Stats(this,"Date","Incidents"),QIcon(":/files/images/statistics.png"),"Statistiques");
    //    ui->tabWidget->setCurrentIndex(incidents_count);
    //    incidents_count++;
}

void MainWindow::on_tableView_Travaux_doubleClicked(const QModelIndex &index)
{
    QSettings settings("Redanium", "Oso Manager");
    Q_UNUSED(index)
    QStandardItemModel *modelMeo,*modelF ;
    Form_incident *currentFormIncident =(Form_incident*) ui->mdiArea->activeSubWindow()->widget();
    Incident_obj *incident=(Incident_obj*)(currentFormIncident->getIncident());

    QLCDNumber *lcd0=currentFormIncident->findChild<QLCDNumber*>("lcdNumber");
    QLCDNumber *lcd1=currentFormIncident->findChild<QLCDNumber*>("lcdNumber_2");
    QAbstractItemModel *AbsModel,*model;

    //QAbstractItemModel *modelList= ui->comboBox->model();

    QString tableName = ui->comboBox->currentText();

    QItemSelectionModel * selection = ui->tableView_Travaux->selectionModel();

    QModelIndexList indexes = selection->selectedIndexes();

    AbsModel =ui->tableView_Travaux->model();
    //QMouseEvent *mouseEvt ;

    if(QApplication::mouseButtons()==Qt::LeftButton)  {

        if(AbsModel->objectName()=="model_employer")

        {


            model = qobject_cast<EmployerModel *>(AbsModel);
            if(tableName.contains("Stock_Entreprise"))

            {
                QStandardItemModel *fournitures_model= (QStandardItemModel*) ui->mdiArea->activeSubWindow()->widget()->findChild<QTableView*>("tableView_fournitures")->model();
               QString key,qty="#1";
                if(fournitures_model!=0)
                {

                    //                   fournitures_model->setHeaderData(0, Qt::Horizontal, QObject::tr("Nom"));
                    //                   fournitures->setHeaderData(1, Qt::Horizontal, QObject::tr("Prénom"));


                    foreach (QModelIndex inx,indexes) {


                        if(inx.column()==0)
                            if(!(model->data(inx).toString().isEmpty()))
                            {    key=model->data(inx).toString();
                                if(incident->getI_fournitures().contains(key)) return;
                                fournitures_model->insertRow(0);

                                incident->add_Fourniture(key,QString(qty).toInt());

                                fournitures_model->setData(fournitures_model->index(0,0),model->data(inx).toString()) ;

                                qDebug()<<"######## ################";
                                break;
                            }
                        if(inx.column()==1)
                            if(!(model->data(inx).toString().isEmpty())){
                                 key=model->data(inx).toString();
                                 if(incident->getI_fournitures().contains(key)) return;
                                 fournitures_model->insertRow(0);

                                 incident->add_Fourniture(key,QString(qty).toInt());
                                fournitures_model->setData(fournitures_model->index(0,0),model->data(inx).toString()) ;

                            }


                    }
                    fournitures_model->setData(fournitures_model->index(0,1),1) ;
incident->i_Preview(2,ui->webView_2);
                    //QTableView *table_fournitures =(QTableView*)ui->mdiArea->activeSubWindow()->widget()->findChild<QTableView*>("tableView_fournitures");
                    //table_fournitures->setModel(fournitures_model);
                }
            }

            if(tableName.contains("Ressources_Humaines"))

            {
                QStandardItemModel *presence_model ;

                presence_model= (QStandardItemModel*) ui->mdiArea->activeSubWindow()->widget()->findChild<QTableView*>("tableView_presence")->model();

                if(presence_model!=0)
                {

                    presence_model->setHeaderData(0, Qt::Horizontal, QObject::tr("Nom"));
                    presence_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Prénom"));

                    QString str;

                    //if( presence_model->match(presence_model->index(0,inx.column()),Qt::DisplayRole,model->data(inx,Qt::DisplayRole ).toString()).size()<1){
                    foreach (QModelIndex inx,indexes) {
                        if(inx.column() < 2){

                            //    presence_model->setData(presence_model->index(0,inx.column()),model->data(inx,Qt::DisplayRole ).toString()) ;
                            str.append(model->data(inx,Qt::DisplayRole ).toString()+"#");
                        }
                    }

                    if( FindItemRow(str.split("#")[0],ui->mdiArea->activeSubWindow()->widget()->findChild<QTableView*>("tableView_presence"))==-1 ){
                        presence_model->insertRow(0);
                        presence_model->setData(presence_model->index(0,0),str.split("#")[0]) ;
                        presence_model->setData(presence_model->index(0,1),str.split("#")[1]) ;
                        incident->add_Presence(str);


                    }

                    QTableView *table_presebce =(QTableView*)ui->mdiArea->activeSubWindow()->widget()->findChild<QTableView*>("tableView_presence");
                    table_presebce->setModel(presence_model);
                }
            }


            else
                if( ((EmployerModel*)model)->IMAGE_MODEL==0 && ((EmployerModel*)model)->tableName().contains("Actes")  ){
                    int row_id_acte;
                    /*/////////////////////////////////////////////////////////////////////////////////////////////////////*/


                    qDebug()<< "incident current objet" << incident->get_i_Objet();

                    QTableView *ttable_meo = ui->mdiArea->activeSubWindow()->widget()->findChild<QTableView*>("tableView_MEO");
                    QTableView *ttable_F =ui->mdiArea->activeSubWindow()->widget()->findChild<QTableView*>("tableView_Fournitures");


                    modelMeo= (QStandardItemModel*) ttable_meo->model();
                    modelF= (QStandardItemModel*) ttable_F->model();
                    QItemSelectionModel * selection = ui->tableView_Travaux->selectionModel();
                    QModelIndexList indexes = selection->selectedIndexes();

                    QStringList temp_list_items ;


                    for( QList<QModelIndex>::iterator  i=indexes.begin();i!=indexes.end();i++){
                        row_id_acte =(*i).row();
                        switch ((*i).column()) {
                        case 0:

                            temp_list_items.append((*i).data(Qt::DisplayRole).toString());
                            qDebug()<< "Designation from list"<<temp_list_items.at(0);
                            break;
                        case 1:

                            temp_list_items.append((*i).data(Qt::DisplayRole).toString());
                            qDebug()<< "Unité from list"<<temp_list_items.at(1);
                            break;

                        case 2:
                            //MEO
                            temp_list_items.append(QString::number((*i).data(Qt::DisplayRole).toDouble(),'f',2));
                            qDebug()<<" Mise en Ouevre from List :"<<temp_list_items.at(2);
                            break;
                        case 3:
                            //Fournitures

                            temp_list_items.append(QString::number((*i).data(Qt::DisplayRole).toDouble(),'f',2));
                            qDebug()<<" Fournitures from List :"<<temp_list_items.at(3);

                            break;
                        default:
                            break;
                        }
                    }

                    double MEO=temp_list_items.at(2).toDouble();
                    double Fournitures=temp_list_items.at(3).toDouble();
                    qDebug()<< "starting function doubleclick debug";
                    qDebug()<< "MEO : " << MEO;
                    qDebug()<< "Fournitures : " << Fournitures;
                    QString tableName=((EmployerModel*)ui->tableView_Travaux->model())->tableName();

                    Incident_Travaux::Acte acte_temp;
                    int Quantity_i_acte=1;

                    tableName.normalized(QString::NormalizationForm_D);
                    acte_temp.append(tableName).append("#"+QString::number(row_id_acte)).append("#"+QString::number(Quantity_i_acte)); //value;

                    if( MEO!=0.0 )
                    {

                        if( Fournitures!=0.0 )

                        {



                            if( incident->getI_Travaux()->getFournitures().size()>1 ) {
                                QStringList listActe = incident->getI_Travaux()->getFournitures();
                                for ( int i= 1;i<listActe.size();i++)
                                {

                                    QStringList currentActe =listActe[i].split("#");

                                    int QtyFromList =currentActe[2].toInt();

                                    if (QtyFromList!=0)

                                    {


                                        int tempRowId=currentActe[1].toInt();

                                        if(tempRowId==row_id_acte && tableName.contains(currentActe[0]))
                                        {

                                            Quantity_i_acte =QtyFromList+1;

                                            acte_temp.clear();
                                            acte_temp.append(tableName).append("#"+QString::number(row_id_acte)+"#"+QString::number(Quantity_i_acte));

                                            incident->getI_Travaux()->deleteAt(i,0);

                                            modelMeo->setData(modelMeo->index(FindItemRow(temp_list_items.at(0),ttable_meo),1),QString::number(Quantity_i_acte),Qt::DisplayRole);
                                            modelF->setData(modelF->index(FindItemRow(temp_list_items.at(0),ttable_F),1),QString::number(Quantity_i_acte),Qt::DisplayRole);
                                            modelMeo->setData(modelMeo->index(FindItemRow(temp_list_items.at(0),ttable_meo),4),QString::number(Quantity_i_acte*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble()*temp_list_items.at(2).toDouble(),'f',2),Qt::DisplayRole);
                                            modelF->setData(modelF->index(FindItemRow(temp_list_items.at(0),ttable_F),4),QString::number(Quantity_i_acte*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble()*temp_list_items.at(3).toDouble(),'f',2),Qt::DisplayRole);


                                            incident->getI_Travaux()->addActe(0,acte_temp);
                                            lcd0->display(QString::number(incident->getI_Travaux()->getFournitures().size()-1));
                                            lcd1->display(QString::number(incident->getI_Travaux()->getMEO().size()-1));
                                            currentFormIncident->updateMoney();
                                            return;

                                        }

                                    }





                                }
                            }



                            incident->getI_Travaux()->addActe(0,acte_temp);
                            lcd0->display(QString::number(incident->getI_Travaux()->getFournitures().size()-1));
                            lcd1->display(QString::number(incident->getI_Travaux()->getMEO().size()-1));
                            //add to tableViews
                            modelMeo->insertRow(0);
                            modelMeo->setData(modelMeo->index(0, 0), temp_list_items.at(0));
                            modelMeo->setData(modelMeo->index(0, 1),   Quantity_i_acte);
                            modelMeo->setData(modelMeo->index(0, 2),  temp_list_items.at(1));
                            modelMeo->setData(modelMeo->index(0, 3), QString::number(temp_list_items.at(2).toDouble()*1*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble(),'f',2));
                            modelMeo->setData(modelMeo->index(0, 4), QString::number(temp_list_items.at(2).toDouble()*Quantity_i_acte*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble(),'f',2));

                            modelF->insertRow(0);
                            modelF->setData(modelF->index(0, 0), temp_list_items.at(0));
                            modelF->setData(modelF->index(0, 1),   Quantity_i_acte);
                            modelF->setData(modelF->index(0, 2),  temp_list_items.at(1));
                            modelF->setData(modelF->index(0, 3), QString::number(temp_list_items.at(3).toDouble()*1*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble(),'f',2));
                            modelF->setData(modelF->index(0, 4), QString::number(temp_list_items.at(3).toDouble()*Quantity_i_acte*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble(),'f',2));
                            currentFormIncident->updateMoney();
                        }
                        else // fourniture == 0
                        {
                            if( incident->getI_Travaux()->getMEO().size()>1 ) {
                                QStringList listActe = incident->getI_Travaux()->getMEO();
                                for ( int i= 1;i<listActe.size();i++)
                                {

                                    QStringList currentActe =listActe[i].split("#");

                                    int QtyFromList =currentActe[2].toInt();

                                    if (QtyFromList!=0)

                                    {


                                        int tempRowId=currentActe[1].toInt();

                                        if(tempRowId==row_id_acte && tableName.contains(currentActe[0]))
                                        {

                                            Quantity_i_acte =QtyFromList+1;

                                            acte_temp.clear();
                                            acte_temp.append(tableName).append("#"+QString::number(row_id_acte)+"#"+QString::number(Quantity_i_acte));

                                            incident->getI_Travaux()->deleteAt(i,1);

                                            modelMeo->setData(modelMeo->index(FindItemRow(temp_list_items.at(0),ttable_meo),1),QString::number(Quantity_i_acte),Qt::DisplayRole);
                                            modelMeo->setData(modelMeo->index(FindItemRow(temp_list_items.at(0),ttable_meo),4),QString::number(Quantity_i_acte*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble()*temp_list_items.at(2).toDouble(),'f',2),Qt::DisplayRole);

                                            incident->getI_Travaux()->addActe(1,acte_temp);
                                            lcd0->display(QString::number(incident->getI_Travaux()->getFournitures().size()-1));
                                            lcd1->display(QString::number(incident->getI_Travaux()->getMEO().size()-1));
                                            currentFormIncident->updateMoney();
                                            return;

                                        }

                                    }





                                }
                            }

                            incident->getI_Travaux()->addActe(1,acte_temp);
                            lcd0->display(QString::number(incident->getI_Travaux()->getFournitures().size()-1));
                            lcd1->display(QString::number(incident->getI_Travaux()->getMEO().size()-1));
                            //add to tableViews
                            modelMeo->insertRow(0);
                            modelMeo->setData(modelMeo->index(0, 0), temp_list_items.at(0));
                            modelMeo->setData(modelMeo->index(0, 1),   Quantity_i_acte);
                            modelMeo->setData(modelMeo->index(0, 2),  temp_list_items.at(1));
                            modelMeo->setData(modelMeo->index(0, 3), QString::number(temp_list_items.at(2).toDouble()*1*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble(),'f',2));
                            modelMeo->setData(modelMeo->index(0, 4), QString::number(temp_list_items.at(2).toDouble()*Quantity_i_acte*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble(),'f',2));
                            currentFormIncident->updateMoney();

                        }

                    }
                    else //MEO == 0
                    {
                        if( incident->getI_Travaux()->getFournitures().size()>1 ) {
                            QStringList listActe = incident->getI_Travaux()->getFournitures();
                            for ( int i= 1;i<listActe.size();i++)
                            {

                                QStringList currentActe =listActe[i].split("#");

                                int QtyFromList =currentActe[2].toInt();

                                if (QtyFromList!=0)

                                {


                                    int tempRowId=currentActe[1].toInt();

                                    if(tempRowId==row_id_acte && tableName.contains(currentActe[0]))
                                    {

                                        Quantity_i_acte =QtyFromList+1;

                                        acte_temp.clear();
                                        acte_temp.append(tableName).append("#"+QString::number(row_id_acte)+"#"+QString::number(Quantity_i_acte));

                                        incident->getI_Travaux()->deleteAt(i,2);

                                        modelF->setData(modelF->index(FindItemRow(temp_list_items.at(0),ttable_F),1),QString::number(Quantity_i_acte),Qt::DisplayRole);
                                        modelF->setData(modelF->index(FindItemRow(temp_list_items.at(0),ttable_F),4),QString::number(Quantity_i_acte*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble()*temp_list_items.at(2).toDouble(),'f',2),Qt::DisplayRole);

                                        incident->getI_Travaux()->addActe(2,acte_temp);
                                        currentFormIncident->updateMoney();


                                        return;

                                    }

                                }





                            }
                        }



                        incident->getI_Travaux()->addActe(2,acte_temp);
                        modelF->insertRow(0);
                        modelF->setData(modelF->index(0, 0), temp_list_items.at(0));
                        modelF->setData(modelF->index(0, 1),   Quantity_i_acte);
                        modelF->setData(modelF->index(0, 2),  temp_list_items.at(1));
                        modelF->setData(modelF->index(0, 3), QString::number(temp_list_items.at(3).toDouble()*Quantity_i_acte*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble(),'f',2));
                        modelF->setData(modelF->index(0, 4), QString::number(temp_list_items.at(3).toDouble()*Quantity_i_acte*(currentFormIncident->getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble(),'f',2));
                        currentFormIncident->updateMoney();

                    }








                }




        }
        else if (AbsModel->objectName()==NULL){qDebug()<<"inherits" << " true " <<AbsModel->metaObject()->className();  return;}
        currentFormIncident->updateMoney();
    }
    else
    {

        currentFormIncident->updateMoney();


    }
    QSoundEffect *sound = new QSoundEffect;
    sound->setSource(QUrl("qrc:/files/data/add.wav"));
    sound->play();
    currentFormIncident->updateMoney();

}

void MainWindow::on_actionIncident_triggered()
{
    Form_incident *incident_ui = new Form_incident(this);
    currentActiveForm=incident_ui;
    ui->actionApercu->setEnabled(true);
    ui->menuImprimer->setEnabled(true);
    ui->actionSur_disque->setEnabled(true);
    ui->actionSur_base_de_donn_es->setEnabled(true);
    ui->actionSouvegarder_sous->setEnabled(true);

    incidents.insert(QString("Incident %1").arg(incidents.size()),incident_ui);

    incident_ui->setWindowTitle(QString("Incident %1").arg(incidents.size()-1));
    QAction *act =new QAction(incident_ui->windowTitle(),this);
    ui->menuWindow->insertAction(0,act);
    ui->mdiArea->addSubWindow(incident_ui);

    incident_ui->showNormal();
    ui->mdiArea->show();
    ui->mdiArea->currentSubWindow()->setWindowIcon(QIcon(":/files/images/incident.png"));

    ui->tableView_Travaux->setEnabled(true);
    connect(incident_ui,SIGNAL(destroyed(QObject*)),this,SLOT(clean(QObject*)));
    connect(act, SIGNAL(triggered()), windowMapper, SLOT(map()));
    windowMapper->setMapping(act,  ui->mdiArea->currentSubWindow());
    ui->tabWidget->tabBar()->setTabEnabled(0,true);


}

//void MainWindow::on_tabWidget_tabCloseRequested(int index)
//{
//    ui->tabWidget->setCurrentIndex(index);
//    if(ui->tabWidget->widget(index)->inherits("Form_incident")){

//        QMessageBox *msgSave= new QMessageBox;
//        msgSave->setText("Voulez-vous sauvegarder");
//        msgSave->setStandardButtons(QMessageBox::Save |  QMessageBox::Cancel);
//        msgSave->setDefaultButton(QMessageBox::Save);
//        msgSave->setButtonText(QMessageBox::Save,"Sauvegarder");
//        msgSave->setButtonText(QMessageBox::Cancel,"Annuler");
//        QMessageBox *msgSave0= new QMessageBox;
//        msgSave0->setText("Choisisez un emplacement");
//        msgSave0->setStandardButtons(QMessageBox::Save |  QMessageBox::Cancel);
//        msgSave0->setDefaultButton(QMessageBox::Save);
//        msgSave0->button(QMessageBox::Save)->setIcon(QIcon(":/files/images/db.png")        );
//        msgSave0->button(QMessageBox::Cancel)->setIcon(QIcon(":/files/images/hd.jpg")        );
//        msgSave0->setButtonText(QMessageBox::Cancel,"dd");
//        msgSave0->setButtonText(QMessageBox::Save,"bdd");
//        if(msgSave->exec()==QMessageBox::Save)
//        {
//            if(msgSave0->exec()==QMessageBox::Save)
//                ui->actionSur_base_de_donn_es->trigger();
//            else /*if(msgSave0->exec()==QMessageBox::Cancel)*/ui->actionSur_disque->trigger();

//            ui->tabWidget->removeTab(index);
//            incidents_count --;

//            incidents.remove(QString::number(index));
//            //delete
//            incidents[QString::number(index)]->deleteLater();
//        }
//        else  {

//            ui->tabWidget->removeTab(index);
//            incidents_count --;

//            incidents.remove(QString::number(index));
//            incidents[QString::number(index)]->deleteLater();
//        }

//    }
//    else if(ui->tabWidget->widget(index)->inherits("incidentsForm")) {
//        QAction *incidentsAction=toolbars.at(0)->actions().at(3);
//        incidentsAction->setEnabled(true);
//        ui->tabWidget->removeTab(index);



//    }




//}

void MainWindow::on_actionImprimer_2_triggered()
{
    Form_incident *currentFormIncident =(Form_incident*) ui->mdiArea->activeSubWindow()->widget();
    Incident_obj *incident=currentFormIncident->getIncident();
    currentActiveForm = currentFormIncident;

    QPrinter *printer= new QPrinter(QPrinter::HighResolution);
    QDialog *dialog= new QDialog(this);
    QVBoxLayout *boxLayout = new QVBoxLayout(dialog);
    QLabel *date_facture = new QLabel("Date de facture :");

    QDateEdit *datefac = new QDateEdit;
    datefac->setDate(QDate::currentDate());
    datefac->setDisplayFormat("dd/MM/yyyy");
    datefac->setCalendarPopup(true);

    QLabel *numero_facture = new QLabel("Numéro de facture :");
    QLineEdit *num_fac = new QLineEdit;
    boxLayout->addWidget(date_facture);
    boxLayout->addWidget(datefac);
    boxLayout->addWidget(numero_facture);
    boxLayout->addWidget(num_fac);
    QHBoxLayout *hl=new QHBoxLayout;
    QPushButton *okBtn = new QPushButton("Ok");
    okBtn->setAutoDefault(true);
    QPushButton *cancelBtn = new QPushButton("Annler");
    hl->addWidget(okBtn);
    hl->addWidget(cancelBtn);
    boxLayout->addLayout(hl);
    dialog->setLayout(boxLayout);
    dialog->setWindowTitle("Facture");
    connect(okBtn,SIGNAL(clicked()),dialog,SLOT(accept()));
    connect(cancelBtn,SIGNAL(clicked()),dialog,SLOT(reject()));
    if(dialog->exec()==QDialog::Accepted)
    {

        incident->setI_Date_Facture(datefac->text());
        incident->setI_numero_facture(num_fac->text().toInt());
        EmployerModel * model = new EmployerModel(this,manager->getDatabase(),0);
        model->setTable("\"Factures\"");
        model->select();
        model->insertRow(0);
        model->setData(model->index(0,0),incident->getI_numero_facture());
        model->setData(model->index(0,1),incident->getI_Date_Facture());
        model->setData(model->index(0,2),incident->getType()==Form_incident::INCIDENT ? "x":" ");
        model->setData(model->index(0,3),incident->getType()==Form_incident::ENTRETIEN ? "x":" ");
        model->submit();

        if(ui->tabWidget_3->currentIndex()==0) currentActiveForm->getIncident()->i_Preview(0,ui->webView_2);
        else    if(ui->tabWidget_3->currentIndex()==1)     currentActiveForm->getIncident()->i_Preview(1,ui->webView_2);
        else  currentActiveForm->getIncident()->i_Preview(2,ui->webView_2);
        statusBar()->showMessage(Print(printer)==1?"Impression avec succes":"Echec de l'impression");
        model->select();
        model->deleteLater();
    }

}

void MainWindow::on_actionSauvegarder_triggered()
{
    QString filename(((Form_incident*) ui->mdiArea->activeSubWindow()->widget())->saveToFile());
    ui->mdiArea->activeSubWindow()->setWindowTitle(QFileInfo(filename).baseName().append(" (sauvegarder a ").append(QTime::currentTime().toString("HH:mm")).append(")"));
    this->statusBar()->showMessage(QString("Incident sauvegardé %1").arg(filename),3000);
}

void MainWindow::on_actionOuvrir_triggered()
{
    ui->actionApercu->setEnabled(true);
    ui->menuImprimer->setEnabled(true);
    ui->actionSur_base_de_donn_es->setEnabled(true);
    ui->actionSouvegarder_sous->setEnabled(true);
    ui->actionSur_disque->setEnabled(true);

    Form_incident *incident_ui = new Form_incident(this);
    QString fileName(QFileInfo( incident_ui->loadFromFile() ).baseName().append("  "));
    incident_ui->setWindowTitle(fileName);
    incident_ui->findChild<QLCDNumber*>("lcdNumber")->display(QString::number(incident_ui->getIncident()->getI_Travaux()->getFournitures().size()));
    incident_ui->findChild<QLCDNumber*>("lcdNumber_2")->display(QString::number(incident_ui->getIncident()->getI_Travaux()->getMEO().size()));


    ui->mdiArea->addSubWindow(incident_ui);
    incident_ui->showMaximized();
    incidents[QString::number(incidents.size())]=incident_ui;

    this->showMaximized();

}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{  //check for the current think about
    QStackedWidget* stackedWidget=0;
    Form_incident *currentFormIncident=0;
    QAbstractItemModel *model;
    EmployerModel *all_model ;
    QString tablename  = arg1;

    if(0!=ui->mdiArea->activeSubWindow())
    {
        currentFormIncident =qobject_cast<Form_incident*>(ui->mdiArea->activeSubWindow()->widget());
        stackedWidget =  qobject_cast<QStackedWidget*>( currentFormIncident->findChild<QStackedWidget*>("stackedWidget"));
    }
    model= ui->comboBox->model();

    if ( tablename.contains("Ressources_Humaines") || tablename.contains("Stock_Entreprise")){

        all_model = new EmployerModel(this, manager->getDatabase(),1);

        all_model->setTable(tablename.contains("Ressources_Humaines")?"Ressources_Humaines":"Stock_Entreprise");
        all_model->select();

        ui->tableView_Travaux->setModel(all_model);


        if(stackedWidget!=NULL){
            if(stackedWidget->currentIndex()!=0 )
                stackedWidget->setCurrentIndex(0);
        }

        return;
    }

    all_model = new EmployerModel(this, manager->getDatabase(),0);

    //a trick for sqlite naming tables convention
    tablename.push_front("\"");
    tablename.push_back("\"");

    if (tablename.contains("Agence")||tablename.contains("Agents") ){


        all_model->setTable(tablename);
        all_model->select();
        ui->tableView_Travaux->setModel(all_model);

        QStringList headersList ;
        for (int i = 0; i < all_model->columnCount(); ++i) {
            headersList<< all_model->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()  ;
        }

        toolbars.at(2)->findChild<QComboBox*>("comboBox_toolbar")->setModel(new QStringListModel(headersList));

        return;
    }


    else

    {
        all_model->setTable(tablename);
        all_model->select();
        ui->tableView_Travaux->setModel(all_model);
        QStringList headersList ;
        for (int i = 0; i < all_model->columnCount(); ++i) {
            headersList<< all_model->headerData(i,Qt::Horizontal,Qt::DisplayRole).toString()  ;
        }

        toolbars.at(2)->findChild<QComboBox*>("comboBox_toolbar")->setModel(new QStringListModel(headersList));
    }

}

void MainWindow::on_actionOptions_triggered()
{

    dialog= new configDialog(this)  ;

    dialog->setWindowTitle("Option");
    readOptions();
    connect(dialog->findChild<QDialogButtonBox*>("buttonBox")->button(QDialogButtonBox::Ok),SIGNAL(clicked()),dialog,SLOT(accept()));
    connect(dialog->findChild<QDialogButtonBox*>("buttonBox")->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),dialog,SLOT(reject()));
    connect(dialog->findChild<QDialogButtonBox*>("buttonBox")->button(QDialogButtonBox::Save),SIGNAL(clicked()),this,SLOT(saveOptions()));

    dialog->exec();
    readOptions();
}

void MainWindow::on_actionSur_base_de_donn_es_triggered()
{
    Form_incident *currentFormIncident =(Form_incident*) ui->mdiArea->activeSubWindow()->widget();
    Incident_obj *tmp_incident=currentFormIncident->getIncident();
    EmployerModel *model=new EmployerModel(this,manager->getDatabase());
    model->setTable("INCIDENTS");
    model->select();
    model->insertRow(0);
    model->setData(model->index(0, 1),tmp_incident->get_i_Objet() );
    model->setData(model->index(0, 2),tmp_incident->getI_Direction() );


    model->setData(model->index(0, 3),tmp_incident->get_i_Agence() );
    model->setData(model->index(0, 4),tmp_incident->get_i_Date() );
    model->setData(model->index(0, 5),tmp_incident->getI_Date_Cmd() );
    model->setData(model->index(0, 6),tmp_incident->getI_Date_Facture() );
    model->setData(model->index(0, 7),tmp_incident->get_i_Heure_debut_fin().join("<--->")  );
    model->setData(model->index(0, 8),tmp_incident->get_i_Commande() );
    model->setData(model->index(0, 9), tmp_incident->get_i_Convention() );

    model->setData(model->index(0, 10),tmp_incident->getI_Travaux()->getAll().join("<--->"));

    model->setData(model->index(0, 11),tmp_incident->getI_Presence().join("<--->")  );
    model->setData(model->index(0, 12), tmp_incident->get_i_Agent().join("\n") );
    model->setData(model->index(0, 13),tmp_incident->get_i_Montant() );
    //    model->setData(model->index(0, 1),"tmp_incident->get_i_Objet()");
    model->setData(model->index(0, 14),tmp_incident->getI_notes().join("\n#########\n") );
    model->setData(model->index(0, 15),currentFormIncident->getF_form_type()==Form_incident::INCIDENT ? "Incident":"Entretien" );



    ui->statusBar->showMessage("Incident enregistré dans la Bdd.",3000);

    model->submit();
}

void MainWindow::on_actionSur_disque_triggered()
{
    QString filename(((Form_incident*) ui->mdiArea->activeSubWindow()->widget())->saveToFile());
    //ui->mdiArea->activeSubWindow()->widget()->setWindowTitle(QFileInfo(filename).baseName().append(" (sauvegarder a ").append(QTime::currentTime().toString("HH:mm")).append(")"));
    this->statusBar()->showMessage(QString("Incident sauvegardé %1").arg(filename),3000);
}

void MainWindow::on_mdiArea_subWindowActivated(QMdiSubWindow *arg1)
{
    if(arg1) {
        Form_incident* widget2 =qobject_cast<Form_incident*>(arg1->widget());
        Incidents_Form *widget3 =qobject_cast<Incidents_Form*>(arg1->widget());

        if(widget2) {

            ui->actionApercu->setEnabled(true);
            ui->actionSur_base_de_donn_es->setEnabled(true);
            ui->actionSur_disque->setEnabled(true);
            if(widget2->getF_form_type()==Form_incident::INCIDENT)
            {
                ui->tabWidget_3->tabBar()->setTabEnabled(1,true);
                ui->tabWidget_3->tabBar()->setTabEnabled(0,true);


            }
            else       {

                ui->tabWidget_3->tabBar()->setTabEnabled(1,false);
                ui->tabWidget_3->tabBar()->setTabEnabled(0,true);


            }



            ui->menuImprimer->setEnabled(true);
            ui->tableView_Travaux->setEnabled(true);
            currentActiveForm=widget2;
            if(ui->tabWidget_3->currentIndex()==0) currentActiveForm->getIncident()->i_Preview(0,ui->webView_2);
            else    if(ui->tabWidget_3->currentIndex()==1)     currentActiveForm->getIncident()->i_Preview(1,ui->webView_2);
            else  currentActiveForm->getIncident()->i_Preview(2,ui->webView_2);

            toolbars.at(1)->setVisible(true);

        }
        else {
            if(widget3) {
                ui->tabWidget_3->tabBar()->setTabEnabled(1,false);
                ui->tabWidget_3->tabBar()->setTabEnabled(0,false);
                ui->actionApercu->setEnabled(false);
                ui->actionSur_base_de_donn_es->setEnabled(false);
                ui->actionSur_disque->setEnabled(false);
                ui->actionSouvegarder_sous->setEnabled(false);

                ui->menuImprimer->setEnabled(false);
                ui->tableView_Travaux->setEnabled(false);//ignore dbClick event and let the table enabled
                toolbars.at(1)->setVisible(false);

            }
            else   {

                ui->actionApercu->setEnabled(false);
                ui->actionSur_base_de_donn_es->setEnabled(false);
                ui->actionSur_disque->setEnabled(false);
                ui->actionSouvegarder_sous->setEnabled(false);
                ui->menuImprimer->setEnabled(false);
                ui->tableView_Travaux->setEnabled(false);

            }
        }
        //if(widget2->closeEvent());
    }


}

void MainWindow::on_actionCascade_triggered()
{
    if(!ui->mdiArea->subWindowList().isEmpty()){
        foreach (QMdiSubWindow *var, ui->mdiArea->subWindowList()) {
            var->showNormal();
        }
        ui->mdiArea->cascadeSubWindows();
    }
}

void MainWindow::on_actionMinimiser_triggered()
{
    if(!ui->mdiArea->subWindowList().isEmpty()){

        foreach (QMdiSubWindow *var, ui->mdiArea->subWindowList()) {
            if(var->windowState()!=Qt::WindowMinimized)
            {

                var->showMinimized();
            }

        }
    }
}

void MainWindow::on_actionMaximiser_triggered()
{
    if(!ui->mdiArea->subWindowList().isEmpty()){
        foreach (QMdiSubWindow *var, ui->mdiArea->subWindowList()) {
            var->showNormal();
        }
        ui->mdiArea->tileSubWindows();
    }
}

void MainWindow::on_actionSuivant_triggered()
{
    if(!ui->mdiArea->subWindowList().isEmpty())   ui->mdiArea->activateNextSubWindow();
}

void MainWindow::on_actionPr_cedent_triggered()
{
    if(!ui->mdiArea->subWindowList().isEmpty()) ui->mdiArea->activatePreviousSubWindow();
}

void MainWindow::on_actionFermer_triggered()
{
    if(!ui->mdiArea->subWindowList().isEmpty())  ui->mdiArea->closeActiveSubWindow();
    //ui->dockWidget->setEnabled(ui->dockWidget->isEnabled() ? false : true );
}

void MainWindow::on_actionFermer_les_fen_tres_triggered()
{
    if(!ui->mdiArea->subWindowList().isEmpty())  ui->mdiArea->closeAllSubWindows();
    // ui->dockWidget->setEnabled(ui->dockWidget->isEnabled() ? false : true );

}

void MainWindow::on_toolButtonSearch_clicked()
{
    QStringList result;
    //    ui->comboBox->setVisible(ui->comboBox->isVisible()? false : true);
    //    ui->lineEditSearch->setVisible(ui->lineEditSearch->isVisible()? false : true);

    //    QString str;
    //    str.append("Acte LIKE ");
    //    str.append(" '%");
    //    str.append(ui->lineEditSearch->text());
    //    str.append("%' ");


    //    QSqlTableModel *model= new QSqlTableModel(this,manager->getDatabase());
    //    foreach (QString tableName, manager->getDbTables()) {
    //        model->setTable(tableName);
    //        model->select();
    //        model->setFilter(str);
    //        if(model->select()) result.append(tableName);

    //    }
}

void MainWindow::on_action_lock_trigered()
{
    ui->tableView_Travaux->setEditTriggers(ui->tableView_Travaux->editTriggers()==QAbstractItemView::SelectedClicked ? QAbstractItemView::NoEditTriggers:QAbstractItemView::SelectedClicked
                                                                                                                       );

}

void MainWindow::on_lineEditTravaux_editingFinished()
{
    //    QString str;
    //    str.append(ui->comboBox_2->currentText());
    //    if(ui->comboBox_3->currentText().contains("contient"))
    //    {
    //        str.append(" LIKE ");
    //        str.append(" '%");
    //        str.append(ui->lineEditTravaux->text());
    //        str.append("%' ");
    //        ((EmployerModel*)ui->tableView_Travaux->model())->setFilter(str);

    //    }
    //    else {
    //        str.append(ui->comboBox_3->currentText());
    //        str.append("'");
    //        str.append(ui->lineEditTravaux->text());
    //        str.append("'");
    //        ((EmployerModel*)ui->tableView_Travaux->model())->setFilter(str);
    //    }
    //    ui->lineEditTravaux->setStyleSheet(ui->lineEditTravaux->text().isEmpty()? "QLineEdit#lineEditTravaux { background-color: lightyellow }":"QLineEdit#lineEditTravaux { background-color: white }");
    //    ui->label->setText(QString::number(ui->tableView_Travaux->model()->rowCount()));
}

void MainWindow::on_actionImprimer_Pdf_triggered()
{

}

void MainWindow::on_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName();
    ui->lineEdit_2->setText(filename);
}

void MainWindow::on_PdfBtn_clicked()
{

}

void MainWindow::on_toolButton_clicked()
{
    //  ui->stackedWidgetFindTravaux->setVisible(ui->stackedWidgetFindTravaux->isVisible()?false:true);
}

void MainWindow::on_tabWidget_3_currentChanged(int index)
{
    switch (index) {
    case 0:
        ui->webView_2->setUrl(QUrl("qrc:/files/data/template.html"));
        if( !incidents.isEmpty())  currentActiveForm->getIncident()->i_Preview(0,ui->webView_2);

        if(!entretiens.isEmpty()  )  currentActiveForm->getIncident()->i_Preview(0,ui->webView_2);

        break;
    case 1:
        ui->webView_2->setUrl(QUrl("qrc:/files/data/bon_commande_.htm"));
        if( !incidents.isEmpty())   currentActiveForm->getIncident()->i_Preview(1,ui->webView_2);
        if(!entretiens.isEmpty() )   currentActiveForm->getIncident()->i_Preview(1,ui->webView_2);


        break;
    case 2:
        ui->webView_2->setUrl(QUrl("qrc:/files/data/interne.html"));
        if( !incidents.isEmpty())   currentActiveForm->getIncident()->i_Preview(2,ui->webView_2);
        if(!entretiens.isEmpty() )   currentActiveForm->getIncident()->i_Preview(2,ui->webView_2);


        break;
    default:
        break;
    }
}

void MainWindow::on_tabWidget_3_tabBarClicked(int index)
{
    switch (index) {
    case 0:
        ui->webView_2->setUrl(QUrl("qrc:/files/data/template.html"));
        currentActiveForm->getIncident()->i_Preview(0,ui->webView_2);
        break;
    case 1:
        ui->webView_2->setUrl(QUrl("qrc:/files/data/bon_commande_.htm"));
        currentActiveForm->getIncident()->i_Preview(1,ui->webView_2);


        break;
    case 2:
        ui->webView_2->setUrl(QUrl("qrc:/files/data/interne.html"));
        currentActiveForm->getIncident()->i_Preview(2,ui->webView_2);


        break;
    default:
        break;
    }
}

void MainWindow::on_actionSwitch_triggered()
{
    ui->splitter_2->setOrientation(ui->splitter_2->orientation()==Qt::Horizontal?Qt::Vertical:Qt::Horizontal);
}

void MainWindow::checkData(){
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
       connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(handleResponse(QNetworkReply*)));
   QNetworkReply* rep=manager->get(QNetworkRequest(QUrl("http://api.thingspeak.com/channels/20855/feed.json?key=752V6QPQC6W9AEN1&results=1")));

 //   ui->label_3->setPixmap(rep->isFinished()? QPixmap(":/files/images/green.png"):QPixmap(":/files/images/red.png") );

}
void MainWindow::on_toolButton_4_clicked()
{
    QSettings settings("Redanium", "Oso Manager");
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(checkData()));
    timer->start(settings.value("incident/interval_maj").toInt());





}

void MainWindow::handleResponse(QNetworkReply* reply)
{
    reply->deleteLater();

       if(reply->error() == QNetworkReply::NoError) {
           // Get the http status code
           int v = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
           if (v >= 200 && v < 300) // Success
           {
                // Here we got the final reply
               QString replyText = reply->readAll();

               QJsonDocument jsonResponse = QJsonDocument::fromJson(replyText.toUtf8());
               QJsonObject jsonObject = jsonResponse.object();
               QJsonArray jsonArray = jsonObject["feeds"].toArray();
               QStringList listarg;


                   listarg.append(jsonArray.at(0).toObject()["field1"].toString());
                    listarg.append(jsonArray.at(0).toObject()["field2"].toString());
                   updateMarker(listarg);

               ui->statusBar->showMessage(listarg.join("##"));
           }
//           else if (v >= 300 && v < 400) // Redirection
//           {
//               // Get the redirection url
//               QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
//               // Because the redirection url can be relative,
//               // we have to use the previous one to resolve it
//               newUrl = reply->url().resolved(newUrl);

//               QNetworkAccessManager *manager = reply->manager();
//               QNetworkRequest redirection(newUrl);
//               QNetworkReply *newReply = manager->get(redirection);

//               return; // to keep the manager for the next request
//           }
       }
       else
       {
           // Error
       }

       reply->manager()->deleteLater();
}



void MainWindow::updateMarker(QStringList args)
{
    qDebug()<< args;
    if(!args.isEmpty())ui->webView->page()->mainFrame()->evaluateJavaScript(QString("updateLocation(%1,%2,%3);").arg(args.at(0)).arg(args.at(1)).arg(true));

}

void MainWindow::updateLocationtable(QStringList args)
{
    ui->tableView->model()->insertRow(0);
    ui->tableView->model()->setData( ui->tableView->model()->index(0,0),args[0]);
    ui->tableView->model()->setData( ui->tableView->model()->index(0,1),args[1]);
    ui->tableView->model()->setData( ui->tableView->model()->index(0,2),args[2].split("T").at(0));
    ui->tableView->model()->setData( ui->tableView->model()->index(0,3),args[2].split("T").at(1));
    ui->tableView->model()->setData( ui->tableView->model()->index(0,4),args[4]);
    ui->tableView->model()->setData( ui->tableView->model()->index(0,5),args[3]);
    ((QSqlTableModel*)ui->tableView->model())->submit();
    ((QSqlTableModel*)ui->tableView->model())->select() ;
}

void MainWindow::clean(QObject* obj)
{
    qDebug() << "cleaning " << incidents.keys();
    qDebug() << "cleaning " <<((Form_incident*)obj)->windowTitle();
    if(obj->objectName().contains("Form_incident"))
    {
        qDebug() << "cleaning incident";
        if(((Form_incident*)obj)->getF_form_type()==Form_incident::INCIDENT)
        {
            qDebug()<<incidents.remove(((Form_incident*)obj)->windowTitle())<<"incidents size "<<incidents.size();

            foreach (QAction* act, ui->menuWindow->actions()) {
                if(act->text().contains(((Form_incident*)obj)->windowTitle()))
                    ui->menuWindow->removeAction(act);

            }

        }
        else{
            qDebug() << "cleaning entretien";

            qDebug()<<entretiens.remove(((Form_incident*)obj)->windowTitle())<<"entretiens size "<<entretiens.size();

            foreach (QAction* act, ui->menuWindow->actions()) {
                if(act->text().contains(((Form_incident*)obj)->windowTitle()))
                    ui->menuWindow->removeAction(act);


            }


        }
    }
    else Incidents_Form_List=0;
    if(ui->mdiArea->subWindowList().isEmpty()){ ui->tabWidget->tabBar()->setTabEnabled(0,false);


        toolbars.at(1)->setVisible(false);

    }
    if (ui->mdiArea->subWindowList().size()==1 && (ui->mdiArea->subWindowList().at(0)->widget()->inherits("Incidents_Form")))
    {
        ui->tabWidget->tabBar()->setTabEnabled(0,false);
        toolbars.at(1)->setVisible(false);

    }
}

void MainWindow::on_toolButton_6_clicked()
{


    ui->toolButton_6->setStyleSheet(    ui->webView->page()->mainFrame()->evaluateJavaScript(QString("icurrentLocation();")).toBool()
                                        ?"background-color:green;":"background-color:red;");

}

void MainWindow::on_toolButton_7_clicked()
{
    ui->webView->page()->mainFrame()->evaluateJavaScript(QString("clearMarkers();"));

}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    ((QSqlTableModel*)ui->tableView->model())->setFilter(QString("Appareil = \'%1\'").arg(ui->listView->model()->data(index).toString()));
    ((QSqlTableModel*)ui->tableView->model())->select() ;
}

void MainWindow::on_actionEntretien_triggered()
{
    Form_incident *incident_ui = new Form_incident(this,Form_incident::ENTRETIEN);
    currentActiveForm=incident_ui;
    ui->actionApercu->setEnabled(true);
    ui->menuImprimer->setEnabled(true);
    ui->actionSur_disque->setEnabled(true);
    ui->actionSur_base_de_donn_es->setEnabled(true);
    ui->actionSouvegarder_sous->setEnabled(true);
    ui->tabWidget_3->tabBar()->setTabEnabled(1,false);

    entretiens.insert(QString("Entretien %1").arg(entretiens.size()),incident_ui);

    incident_ui->setWindowTitle(QString("Entretien %1").arg(entretiens.size()-1));
    QAction *act =new QAction(incident_ui->windowTitle(),this);
    ui->menuWindow->insertAction(0,act);
    incident_ui->setWindowIcon(QIcon(":/files/images/gear.png"));
    ui->mdiArea->addSubWindow(incident_ui);

    incident_ui->showNormal();
    ui->mdiArea->currentSubWindow()->setWindowIcon(QIcon(":/files/images/gear.png"));
    ui->tableView_Travaux->setEnabled(true);
    connect(incident_ui,SIGNAL(destroyed(QObject*)),this,SLOT(clean(QObject*)));

    connect(act, SIGNAL(triggered()), windowMapper, SLOT(map()));
    windowMapper->setMapping(act,  ui->mdiArea->currentSubWindow());
    ui->tabWidget->tabBar()->setTabEnabled(0,true);

}

void MainWindow::activateSubWindow(QWidget *widget)
{
    QMdiSubWindow* window = (QMdiSubWindow*) widget;
    ui->mdiArea->setActiveSubWindow(window);

}

void MainWindow::on_actionSwitch2_triggered()
{
    ui->splitter->setOrientation(ui->splitter->orientation()==Qt::Horizontal?Qt::Vertical:Qt::Horizontal);

}

void MainWindow::on_actionApercu_triggered()
{
    filePrintPreview();
}
