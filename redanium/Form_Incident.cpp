#include "form_incident.h"
#include"QProgressIndicator.h"
#include<QSoundEffect>

#include "ui_form_incident.h"
#include"incident.h"
#include"mainwindow.h"
#include"i_table_fixedrow.h"
#include<QCalendarWidget>
#include<QStandardItemModel>
#include<QHeaderView>
#include<QNetworkAccessManager>
#include<QNetworkInterface>
#include<QWebFrame>

void Form_incident::updateMoney()
{
    double montant_temp =0;
    QTableView *tableViewMontant = this->findChild<QTableView*>("tableView_Montant");
    QStandardItemModel *model_montant = qobject_cast<QStandardItemModel*>(tableViewMontant->model());
    QTableView *ttable_meo = this->findChild<QTableView*>("tableView_MEO");

    QTableView *ttable_F =this->findChild<QTableView*>("tableView_Fournitures");
    QStandardItemModel   *modelMeo= (QStandardItemModel*) ttable_meo->model();
    QStandardItemModel   *modelF= (QStandardItemModel*) ttable_F->model();
    for(int i=0 ; i<modelF->rowCount() ; i++)
    {
        montant_temp += modelF->data(modelF->index(i,4),Qt::DisplayRole).toDouble();
    }

    for(int i=0;i<modelMeo->rowCount();i++)
    {
        montant_temp += modelMeo->data(modelMeo->index(i,4),Qt::DisplayRole).toDouble();
    }

    model_montant->setData(model_montant->index(0,0),QString::number(montant_temp,'f',2),Qt::DisplayRole); // Total HT
    model_montant->setData(model_montant->index(1,0),QString::number(montant_temp*0.05,'f',2),Qt::DisplayRole); // RG 5%
    model_montant->setData(model_montant->index(2,0),QString::number(montant_temp-montant_temp*0.05,'f',2),Qt::DisplayRole); // Total apres RG 5%
    model_montant->setData(model_montant->index(3,0),QString::number((montant_temp-montant_temp*0.05)*0.17,'f',2),Qt::DisplayRole); // Total 17%
    model_montant->setData(model_montant->index(4,0),QString::number((montant_temp-montant_temp*0.05)*1.17,'f',2),Qt::DisplayRole); // TOTAL TTC
    QLabel *moneyTXT=this->findChild<QLabel*>("MoneyTextEdit");
    moneyTXT->setText(QString("<b><font color=\"green\">Arrêter la présente facture à la somme de :</fon></b>\n\n%1").arg("<b>"+Incident_obj::ChiffreEnLettre((montant_temp-montant_temp*0.05)*1.17)+"</b>"));
    tmp_incident->set_i_Montant(QString::number((montant_temp-montant_temp*0.05)*1.17,'f',2).toDouble());
    iTableFixRow->updateTotal(0);
    iTableFixRow->updateTotal(1);
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));
    qDebug()<<"UPDATING MONEY";
}
Form_incident::Form_incident(QWidget *parent,int form_type) :
    QWidget(parent),
    ui(new Ui::Form_incident)
{
    F_form_type = form_type;
    Oran_wilaya = ( QStringList()
                    << "Arzew (commune)"<<
                    "Aïn Bya"<<
                    "Aïn El Kerma"<<
                    "Aïn El Turk (commune)"<<
                    "Ben Freha"<<
                    "Bethioua (commune)"<<
                    "Bir El Djir (commune)"<<
                    "Boufatis"<<
                    "Bousfer"<<
                    "Boutlélis (commune)"<<
                    "El Ançor"<<
                    "El Braya"<<
                    "El Kerma"<<
                    "Es Sénia (commune)"<<
                    "Gdyel (commune)"<<
                    "Hassi Ben Okba"<<
                    "Hassi Bounif"<<
                    "Hassi Mefsoukh"<<
                    "Mers El Hadjadj"<<
                    "Mers El Kébir"<<
                    "Misserghin"<<
                    "Oran(commune)"<<
                    "Oued Tlélat(commune)"<<
                    "Sidi Ben Yebka" <<
                    "Sidi Chami"<<
                    "Tafraoui"<<
                    "Aïn El Turk(daïra)"<<
                    "Arzew(daïra)"<<
                    "Bethioua (daïra)"<<
                    "Bir El Djir (daïra)"<<
                    "Boutlélis (daïra)"<<
                    "Es Sénia (daïra)"<<
                    "Gdyel (daïra)"<<
                    "Oran (daïra)"<<
                    "Oued Tlélat");
    // incident
    tmp_incident = new Incident_obj(this);
    tmp_incident->setType(getF_form_type());

    //main app window and DB manager
    i_MainWindow=(MainWindow*)parent;
    manager=i_MainWindow->getManager();
    i_MainWindow->findChild<QWebView*>("webView_2")->setUrl(QUrl("qrc:/files/data/template.html"));

    ui->setupUi(this);

    ui->dateEdit->setDate(QDate::currentDate());
    QCalendarWidget *cal=ui->dateEdit->calendarWidget();
    cal->setLocale(QLocale::French);

    ui->dateEdit_2->setDate(QDate::currentDate());
    QCalendarWidget *cal2=ui->dateEdit_2->calendarWidget();
    cal2->setLocale(QLocale::French);

    QStandardItemModel *model1= new QStandardItemModel(0,5,this);
    model1->setHeaderData(0, Qt::Horizontal, QObject::tr("Désignation"));
    model1->setHeaderData(1, Qt::Horizontal, QObject::tr("Quantité"));
    model1->setHeaderData(2, Qt::Horizontal, QObject::tr("Unité"));
    model1->setHeaderData(3, Qt::Horizontal, QObject::tr("P.U en H.T"));
    model1->setHeaderData(4, Qt::Horizontal, QObject::tr("Montant"));
    model1->setHeaderData(5, Qt::Horizontal, QObject::tr("HT"));
    //    ui->tableView->setModel(model1);
    //    ui->tableView->setAlternatingRowColors(true);
    //    ui->tableView->resizeColumnsToContents();
    QStandardItemModel *fournitures_model=new QStandardItemModel(0,2,this) ;
    fournitures_model->setHeaderData(0, Qt::Horizontal, QObject::tr("Réf/Article"));
    fournitures_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Quantité"));
    ui->tableView_fournitures->setModel(fournitures_model);
    ui->tableView_fournitures->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QStandardItemModel *presence_model=new QStandardItemModel(0,2,this) ;
    presence_model->setHeaderData(0, Qt::Horizontal, QObject::tr("Nom"));
    presence_model->setHeaderData(1, Qt::Horizontal, QObject::tr("Prénom"));
    ui->tableView_presence->setModel(presence_model);
    ui->tableView_presence->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    QSqlTableModel *agenceModel = new QSqlTableModel(this,manager->getDatabase());
    agenceModel->setTable("Agence");
    agenceModel->select();
    QStringList agenceList("Agence");
    for (int i = 0; i < agenceModel->rowCount(); ++i) {
        agenceList.append(agenceModel->data(agenceModel->index(i,0),Qt::DisplayRole).toString());
    }
    iTableFixRow=new i_table_fixedrow(this);
    iTableFixRow->setIncident(tmp_incident);
    ui->stackedWidget->insertWidget(1,iTableFixRow);
    connect(((QStandardItemModel*)iTableFixRow->findChild<QTableView*>("tableView_Fournitures")->model()),SIGNAL(itemChanged(QStandardItem*)),tmp_incident->getI_Travaux(),SLOT(ModifyQty(QStandardItem*)));
    connect(((QStandardItemModel*)iTableFixRow->findChild<QTableView*>("tableView_MEO")->model()),SIGNAL(itemChanged(QStandardItem*)),tmp_incident->getI_Travaux(),SLOT(ModifyQty(QStandardItem*)));
    //connect(((QStandardItemModel*)iTableFixRow->findChild<QTableView*>("tableView_Fournitures")->model()),SIGNAL(itemChanged(QStandardItem*)),this,SLOT(updateMoney()));
    //connect(((QStandardItemModel*)iTableFixRow->findChild<QTableView*>("tableView_MEO")->model()),SIGNAL(itemChanged(QStandardItem*)),this,SLOT(updateMoney()));

    ui->comboBox_Agence->setModel(new QStringListModel(agenceList));




    connect(ui->spinBox_Cmd_Num,SIGNAL(editingFinished()),this,SLOT(on_spinBox_Cmd_Num_EditingFinished()));

    QCompleter *completer = new QCompleter(this);
    completer->setModel(new QStringListModel(manager->getDictionary()));
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setCompletionMode(QCompleter::PopupCompletion);

    QCompleter *completer1 = new QCompleter(this);
    completer1->setModel(new QStringListModel(Oran_wilaya));
    completer1->setCaseSensitivity(Qt::CaseInsensitive);
    completer1->setCompletionMode(QCompleter::PopupCompletion);


    connect(ui->plainTextEdit,SIGNAL(textChanged()),this,SLOT(on_textEdit_textChanged()));
    connect(this->findChild<QTableView*>("tableView_Fournitures")->model(),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(updatePreview_()));
    tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    ui->listView->setModel(new QStringListModel);
    ui->label->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    ui->label_3->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    ui->label_4->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    ui->label_5->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    ui->label_6->setVisible(F_form_type==Form_incident::INCIDENT? true:false);

    ui->comboBox_2->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    ui->label_Date->setText(F_form_type==Form_incident::INCIDENT? "Date incident :":"Date entretien :");
    // ui->horizontalLayout_4->(F_form_type==Form_incident::INCIDENT? true:false);
    ui->label_Objet_2->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    ui->dateEdit_2->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    ui->label_Date_2->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    ui->spinBox_Cmd_Num->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    ui->label_7->setVisible(F_form_type==Form_incident::INCIDENT? true:false);
    this->setStyleSheet(F_form_type==Form_incident::INCIDENT?QString(this->styleSheet().replace("rgb(67, 107, 149)","orange")):QString(this->styleSheet().replace("rgb(67, 107, 149)","green")));
    i_MainWindow->findChild<QToolBar*>("toolBar_1")->setVisible(true);
    tmp_incident->setI_webView( i_MainWindow->findChild<QWebView*>("webView_2"));

    connect(ui->Checkablewidget,SIGNAL(addingItem(QString)),tmp_incident,SLOT(addAgent(QString)));
    connect(ui->Checkablewidget,SIGNAL(deletingItem(QObject*)),tmp_incident,SLOT(removeAgent(QObject*)));
    connect(ui->listView->model(),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(updateNotes()));
    connect(((QStandardItemModel*)ui->tableView_fournitures->model()),SIGNAL(itemChanged(QStandardItem*)),tmp_incident,SLOT(QtyFournituresChanged(QStandardItem*)));

}

Form_incident::~Form_incident()
{
    tmp_incident->deleteLater();
    iTableFixRow->deleteLater();
    delete ui;
}

QString Form_incident::loadFromFile()
{

    QString filename = QFileDialog::getOpenFileName();

    if(!filename.isEmpty()){
        tmp_incident->i_DeSerialize(filename);
        setF_form_type(tmp_incident->getType());
        update_Ui_Load();

    }
    else {

        i_MainWindow->statusBar()->showMessage(QString("Erreur d'ouverture du fichier %1 !").arg(filename),2000);
    }
    //wn->UpdateMoney();
    //TO-DO add Ui Table taravaux update
    return filename;
}

void Form_incident::loadFromDatabase(int row){


    EmployerModel* model = new EmployerModel(this,manager->getDatabase());

    model->setTable("INCIDENTS");
    model->select();
    tmp_incident->set_i_NumID(model->data(model->index(row,0),Qt::DisplayRole).toInt());
    tmp_incident->set_i_Objet(model->data(model->index(row,1),Qt::DisplayRole).toString());

    tmp_incident->set_i_Agence(model->data(model->index(row,2),Qt::DisplayRole).toString());

    tmp_incident->set_i_Date(model->data(model->index(row,3),Qt::DisplayRole).toString());

    tmp_incident->setI_Date_Cmd(model->data(model->index(row,4),Qt::DisplayRole).toString());

    tmp_incident->setI_Date_Facture(model->data(model->index(row,5),Qt::DisplayRole).toString());

    tmp_incident->set_i_Commande(model->data(model->index(row,7),Qt::DisplayRole).toInt());
    //travaux
    QStringList travaux;
    travaux=QString(model->data(model->index(row,7),Qt::DisplayRole).toString()).split("<--->");
    foreach (QString acte,travaux ) {
        if(!acte.contains("#Fourniture"))
            tmp_incident->getI_Travaux()->addActe(1,acte);

        else             tmp_incident->getI_Travaux()->addActe(2,acte);

    }

    update_Ui_Load();





}

void Form_incident::update_Ui_Load()
{
    QSettings settings("Redanium", "Oso Manager");
    EmployerModel *model= new EmployerModel(i_MainWindow,manager->getDatabase(),1);
    QStandardItemModel *modelMeo,*modelF ;

    ui->plainTextEdit->setPlainText(tmp_incident->get_i_Objet());

    ui->spinBox_Cmd_Num->setValue(tmp_incident->get_i_Commande());
    ui->dateEdit_2->setDate(QDate::fromString(tmp_incident->getI_Date_Cmd(),"dd/MM/yyyy"));

    ui->dateEdit->setDate(QDate::fromString(tmp_incident->get_i_Date(),"dd/MM/yyyy"));
    QStringList lst;
    for (int i = 0; i < settings.value(QString("general/jourFerier_nbr")).toInt(); ++i) {
        lst.append(settings.value(QString("general/jourFerier_%1").arg(i)).toString());
    }
    if(lst.contains(tmp_incident->get_i_Date())) {
        tmp_incident->setMajoration_Fifty_DayOff(true);
        ui->label_5->setPixmap(QPixmap(":/files/images/green.png"));
    }
    else {tmp_incident->setMajoration_Fifty_DayOff(false);
        ui->label_5->setPixmap(QPixmap(":/files/images/red.png"));
    }

    ui->timeEdit_Debut->setTime(QTime::fromString(tmp_incident->get_i_Heure_debut_fin().at(0),"hh:mm"));
    QString debut(settings.value("general/travail_nuit_debut").toString());
    if( tmp_incident->get_i_Heure_debut_fin().at(0).split(":").at(0).toInt() >= debut.split(":").at(0).toInt()  )
    {
        tmp_incident->setMajoration_Fifty_Night(true);
        ui->label_6->setPixmap(QPixmap(":/files/images/green.png"));
    }

    else
    {
        tmp_incident->setMajoration_Fifty_Night(false);
        ui->label_6->setPixmap(QPixmap(":/files/images/red.png"));
    }

    ui->timeEdit_Fin->setTime(QTime::fromString(tmp_incident->get_i_Heure_debut_fin().at(1),"hh:mm"));

    if(tmp_incident->getI_Direction().contains("Es-Senia")) ui->comboBox_3->setCurrentIndex(1);

    if(tmp_incident->get_i_Agence().contains("Es-Senia"))
        ui->comboBox_Agence->setCurrentIndex(0);

    if(tmp_incident->get_i_Agence().contains("Arzew"))
        ui->comboBox_Agence->setCurrentIndex(1);

    if(tmp_incident->get_i_Agence().contains("Ain"))
        ui->comboBox_Agence->setCurrentIndex(2);

    if(tmp_incident->get_i_Agence().contains("Gdyel"))
        ui->comboBox_Agence->setCurrentIndex(3);

    if(tmp_incident->getI_Direction().contains("Oran")) ui->comboBox_3->setCurrentIndex(2);

    if(tmp_incident->get_i_Agence().contains("Oran"))
        ui->comboBox_Agence->setCurrentIndex(0);
    if(tmp_incident->get_i_Agence().contains("Bir"))
        ui->comboBox_Agence->setCurrentIndex(1);
    switch (tmp_incident->getImputation()) {
    case Incident_obj::AERIEN:
        ui->comboBox_2->setCurrentIndex(1);

        break;
    case Incident_obj::SOUSTERRAIN:
        ui->comboBox_2->setCurrentIndex(2);

        break;
    case Incident_obj::POSTE:
        ui->comboBox_2->setCurrentIndex(3);
        break;
    default:
        break;
    }

    if(!tmp_incident->getI_Presence().isEmpty()){
        for (int i = 0; i < tmp_incident->getI_Presence().size(); ++i) {

            ui->tableView_presence->model()->insertRow(0);
            ui->tableView_presence->model()->setData(ui->tableView_presence->model()->index(0,0),tmp_incident->getI_Presence().at(i).split("#").at(0));
            ui->tableView_presence->model()->setData(ui->tableView_presence->model()->index(0,1),tmp_incident->getI_Presence().at(i).split("#").at(1));
        }
    }
    QTableView *ttable_meo = iTableFixRow->findChild<QTableView*>("tableView_MEO");

    QTableView *ttable_F = iTableFixRow->findChild<QTableView*>("tableView_Fournitures");

    modelMeo= (QStandardItemModel*) ttable_meo->model();
    modelF= (QStandardItemModel*) ttable_F->model();
    //tmp_incident->getI_Travaux()->removeFirst();

    if(!tmp_incident->getI_Travaux()->getMEO().isEmpty()) {
        for(int i = 0; i < tmp_incident->getI_Travaux()->getMEO().size();i++)
        {

            QStringList lista,list=tmp_incident->getI_Travaux()->getMEO();
            if(list.at(i).contains("#MEO")) ;
            else  {
                QString str = list.at(i);

                lista=str.split("#");
                int quantity =lista[2].toInt();
                int rowID = lista[1].toInt();
                QString tableName = lista[0];
                model->setTable(tableName);
                model->select();

                modelMeo->insertRow(0);
                modelMeo->setData(modelMeo->index(0, 0), model->data(model->index(rowID,0),Qt::DisplayRole ));
                modelMeo->setData(modelMeo->index(0, 1),  quantity);
                modelMeo->setData(modelMeo->index(0, 2),  model->data(model->index(rowID,1),Qt::DisplayRole ));
                modelMeo->setData(modelMeo->index(0, 3), model->data(model->index(rowID,3),Qt::DisplayRole ).toDouble()*1*(getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble());
                modelMeo->setData(modelMeo->index(0, 4), model->data(model->index(rowID,3),Qt::DisplayRole ).toDouble()*quantity*(getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble());
            }
        }
    }
    if(!tmp_incident->getI_Travaux()->getFournitures().isEmpty())
    {
        for(int i = 0; i < tmp_incident->getI_Travaux()->getFournitures().size();i++)
        {

            QStringList lista,list=tmp_incident->getI_Travaux()->getFournitures();
            if(list.at(i).contains("#Fournitures")) ;
            else {
                QString str = list.at(i);

                lista=str.split("#");

                int quantity =lista[2].toInt();

                int rowID = lista[1].toInt();
                QString tableName = lista[0];
                model->setTable(tableName);
                model->select();
                modelF->insertRow(0);
                modelF->setData(modelF->index(0, 0), model->data(model->index(rowID,0),Qt::DisplayRole ));
                modelF->setData(modelF->index(0, 1), quantity);
                modelF->setData(modelF->index(0, 2), model->data(model->index(rowID,1),Qt::DisplayRole ));
                modelF->setData(modelF->index(0, 3), model->data(model->index(rowID,4),Qt::DisplayRole ).toDouble()*1*(getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble());
                modelF->setData(modelF->index(0, 4), model->data(model->index(rowID,4),Qt::DisplayRole ).toDouble()*quantity*(getF_form_type()==Form_incident::ENTRETIEN?settings.value("entretien/point_value"):settings.value("incident/point_value")).toDouble());
            }
        }
    }





}

void Form_incident::updatePreview(int type)
{
    QSettings settings("Redanium", "Oso Manager");
    double montantHT=0,montantMeoHT=0;
    QStringList htmlActeMeo,htmlActeF;
    QString t_str;
    QWebView *webview=i_MainWindow->findChild<QWebView*>("webView_2");



    QString majorationRowBoth="<tr >"
            "   <td ><b>Majoration 50% (Jour férier/Travail de nuit)</b></td>"
            "  <td align=\"center\"><b>/</b></td>"
            "    <td align=\"center\"><b>/</b></td>"
            "  <td align=\"center\"><b>%21</b></td>"
            " <td align=\"center\"><b>%21</b></td>"
            "</tr>";
    QString majorationRowDoff="<tr >"
            "   <td ><b>Majoration 50% (Jour férier)</b></td>"
            "  <td align=\"center\"><b>/</b></td>"
            "    <td align=\"center\"><b>/</b></td>"
            "  <td align=\"center\"><b>%21</b></td>"
            " <td align=\"center\"><b>%21</b></td>"
            "</tr>";
    QString majorationRowNight="<tr >"
            "   <td ><b>Majoration 50% (Travail de nuit)</b></td>"
            "  <td align=\"center\"><b>/</b></td>"
            "    <td align=\"center\"><b>/</b></td>"
            "  <td align=\"center\"><b>%21</b></td>"
            " <td align=\"center\"><b>%21</b></td>"
            "</tr>";
    QString MeoHeader ="<tr class=\"odd\">"
            "<td colspan=\"5\" bgcolor=\"lightgrey\"><b>Mise en Oeuvre</b></td>"
            "</tr>";
    QString FournituresHeader ="<tr class=\"odd\">"
            "<td colspan=\"5\" bgcolor=\"lightgrey\"><b>Foutnitures</b></td>"
            "</tr>";


    QString strActe="<tr ><td>%1</td>"
            "<td align=\"center\">%2</td>"
            "<td align=\"center\">%3</td>"
            "<td align=\"center\">%4</td>"
            "<td align=\"center\">%5</td>"
            "</tr>";

    QString head_b_cmd ="<tr>"
            "<td colspan=\"3\"><b>%1</b></td>"

            " </tr>";



    QString b_cmd_acte= "<tr>"
            "<td align=\"left\">%1</td>"
            " <td align=\"center\">%2</td>"
            "<td align=\"center\">%3</td>"
            " </tr>";

    QFile filehtml;
    //open the html template from ressources
    if(type==0)filehtml.setFileName(":/files/data/template.html");
    if(type==1)filehtml.setFileName(":/files/data/bon_commande_.htm");

    if (filehtml.open(QFile::ReadOnly | QFile::Text))
        qDebug()<<"template.html is open : "<<filehtml.isOpen();
    QTextStream in(&filehtml);
    QString str = QString(in.readAll());
    webview->setHtml(str);
    filehtml.close();
    if(type==0){
        QAbstractItemModel *model_f = this->findChild<QTableView*>("tableView_Fournitures")->model();
        if(model_f!=0)
        {
            if(tmp_incident->getI_Travaux()->getFournitures().size()>1){
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
        QAbstractItemModel *model_m = this->findChild<QTableView*>("tableView_MEO")->model();
        if(model_m!=0)
        {
            if(tmp_incident->getI_Travaux()->getMEO().size()>1){
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

                if(tmp_incident->getMajoration_Fifty_DayOff()==tmp_incident->getMajoration_Fifty_Night()){
                    if(tmp_incident->getMajoration_Fifty_DayOff()) {
                        htmlActeMeo.append(majorationRowBoth);
                        montantHT=montantHT+0.5*montantMeoHT;
                    }
                }
                else{
                    if(tmp_incident->getMajoration_Fifty_DayOff()) {
                        htmlActeMeo.append(majorationRowDoff);
                        montantHT=montantHT+0.5*montantMeoHT;

                    }
                    if(tmp_incident->getMajoration_Fifty_Night()){
                        htmlActeMeo.append(majorationRowNight);
                        montantHT=montantHT+0.5*montantMeoHT;

                    }
                }

            }
        }



        webview->setHtml( // 1
                          str.arg(tmp_incident->getI_Date_Facture())
                          // 2
                          .arg(tmp_incident->getI_Direction())
                          // 3
                          .arg(tmp_incident->get_i_Agence())
                          .arg(QString(tmp_incident->get_i_Objet()).prepend(F_form_type==Form_incident::INCIDENT?"Intitul&eacute; de l'incident :":"Intitul&eacute; de l'entretien :"))
                          // 4
                          .arg(tmp_incident->get_i_Date())
                          // 5
                          .arg(Incident_obj::ChiffreEnLettre((montantHT-montantHT*0.05)*0.17+montantHT-montantHT*0.05))
                          //6
                          .arg(htmlActeF.join("<!-- -->"))
                          //7
                          .arg(htmlActeMeo.join("<!-- -->"))
                          //8
                          .arg(settings.value(F_form_type==Form_incident::INCIDENT?"incident/convention_numero":"entretien/convention_numero").toInt())
                          //9
                          .arg(settings.value(F_form_type==Form_incident::INCIDENT?"incident/convention_année":"entretien/convention_année").toInt())
                          //10
                          .arg(F_form_type==Form_incident::INCIDENT?QString::number(tmp_incident->getImputation()).prepend("IMPUTATION 932 1C1 "):QString())
                          //11
                          .arg(F_form_type==Form_incident::INCIDENT?QString("BON DE COMMANDE N&deg; %1").arg(QString::number(tmp_incident->get_i_Commande()).append(" du %13")):QString())
                          //12
                          .arg(tmp_incident->getI_Date_Cmd())
                          //13
                          .arg(tmp_incident->getI_numero_facture())
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
                          //21
                          .arg(QString::number(montantMeoHT*0.5,'f',2))

                          );

    }

    if(type==1){
        QAbstractItemModel *model_f = this->findChild<QTableView*>("tableView_Fournitures")->model();
        if(model_f!=0)
        {
            if(tmp_incident->getI_Travaux()->getFournitures().size()>1){
                htmlActeF.append(b_cmd_acte.arg(head_b_cmd.arg("Fournitures")).arg(" ").arg(" "));
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
        QAbstractItemModel *model_m = this->findChild<QTableView*>("tableView_MEO")->model();
        if(model_m!=0)
        {
            if(tmp_incident->getI_Travaux()->getMEO().size()>1){
                htmlActeMeo.append(b_cmd_acte.arg(head_b_cmd.arg("Mise en Oeuvre")).arg(" ").arg(" "));
                for (int i = 0; i < model_m->rowCount(); ++i) {
                    t_str=b_cmd_acte.arg(model_m->data(model_m->index(i,0)).toString())
                            .arg(model_m->data(model_m->index(i,1)).toString())
                            .arg(model_m->data(model_m->index(i,3)).toString())
                            ;
                    htmlActeMeo.append(t_str);
                    montantHT = montantHT + model_m->data(model_m->index(i,4)).toDouble();
                    montantMeoHT = montantMeoHT + model_m->data(model_m->index(i,4)).toDouble();
                }

                if(tmp_incident->getMajoration_Fifty_DayOff()==tmp_incident->getMajoration_Fifty_Night()){
                    if(tmp_incident->getMajoration_Fifty_DayOff()) {
                        htmlActeMeo.append(b_cmd_acte.arg("Majoration 50% (Jour férier/Travail de nuit)").arg("/").arg(QString::number(montantMeoHT*0.5,'f',2)));
                        montantHT=montantHT+0.5*montantMeoHT;
                    }
                }
                else{
                    if(tmp_incident->getMajoration_Fifty_DayOff()) {
                        htmlActeMeo.append(b_cmd_acte.arg("Majoration 50% (Jour férier)").arg("/").arg(QString::number(montantMeoHT*0.5,'f',2)));
                        montantHT=montantHT+0.5*montantMeoHT;

                    }
                    if(tmp_incident->getMajoration_Fifty_Night()){
                        htmlActeMeo.append(b_cmd_acte.arg("Majoration 50% (Travail de nuit)").arg("/").arg(QString::number(montantMeoHT*0.5,'f',2)));
                        montantHT=montantHT+0.5*montantMeoHT;

                    }
                }

            }
        }
        webview->setHtml( // 1
                          str.arg(tmp_incident->get_i_Commande())
                          // 2
                          .arg(settings.value("incident/convention_année").toInt())
                          // 3
                          .arg(tmp_incident->getI_Date_Cmd())
                          // 4
                          .arg(settings.value(QString("entreprise/nom")).toString())
                          // 5
                          .arg(tmp_incident->get_i_Objet())
                          //6
                          .arg(tmp_incident->get_i_Date())
                          //7
                          .arg(" ")
                          //8
                          .arg(tmp_incident->getImputation())
                          //9
                          .arg(htmlActeF.join("<!-- -->"))
                          //10
                          .arg(htmlActeMeo.join("<!-- -->"))
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






}

void Form_incident::updatePreview_()
{
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));
}

QString Form_incident::saveToFile(){
    QString fileName = QFileDialog::getSaveFileName(this, "Sauvegarder Incident",
                                                    QString(), "*.inc");
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".inc");
        qDebug()<< "incident getting saved,is it null ? "<<(tmp_incident==0);

        tmp_incident->i_Serialize(fileName);
        QApplication::restoreOverrideCursor();
        return fileName;
    }

    else {    QApplication::restoreOverrideCursor();
        return QString("Erreur Fichier. Nom de fichier non spécifié!");
    }

}

void Form_incident::loadStaticMap(bool status)
{
    qDebug()<<"Static Maps 0 " << status ;
    qDebug()<<"Static Maps 1" ;



}


void Form_incident::on_comboBox_Agent_currentIndexChanged(const QString &arg1)
{
    //tmp_incident->set_i_Agent(arg1);
}



void Form_incident::on_dateEdit_editingFinished()
{    QSettings settings("Redanium", "Oso Manager");

     tmp_incident->set_i_Date(ui->dateEdit->date().toString("dd/MM/yyyy"));



      QStringList lst;
       for (int i = 0; i < settings.value(QString("general/jourFerier_nbr")).toInt(); ++i) {
           lst.append(settings.value(QString("general/jourFerier_%1").arg(i)).toString());
       }
        if(lst.contains(tmp_incident->get_i_Date())) {
            tmp_incident->setMajoration_Fifty_DayOff(true);
            ui->label_5->setPixmap(QPixmap(":/files/images/green.png"));
        }
        else {tmp_incident->setMajoration_Fifty_DayOff(false);
            ui->label_5->setPixmap(QPixmap(":/files/images/red.png"));
        }
         if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
         else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
         else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));
}
void Form_incident::on_lineEdit_Objet1_editingFinished()
{



}


void Form_incident::on_comboBox_Agence_currentIndexChanged(int index)
{int indexx=0;
    QSqlTableModel *model = new QSqlTableModel(this,manager->getDatabase());

    model->setTable("Agents");
    if (ui->comboBox_Agence->currentText().contains("Oran") )indexx=1;
    if (ui->comboBox_Agence->currentText().contains("Es-Senia")) indexx=2;
    if (ui->comboBox_Agence->currentText().contains("Arzew")) indexx=3;
    if (ui->comboBox_Agence->currentText().contains("Ain-Turk")) indexx=4;
    if (ui->comboBox_Agence->currentText().contains("Gdyel")) indexx=5;
    if (ui->comboBox_Agence->currentText().contains("Bir el jir")) indexx=6;

    model->setFilter(QString("Agence =\'"+QString::number(indexx)+"\'"));
    model->select();

    QStandardItemModel *model1=new QStandardItemModel(model->rowCount(),0,this); // 3 rows, 1 col
    for (int r = 0; r <model->rowCount(); ++r)
    {
        QStandardItem* item = new QStandardItem(model->data(model->index(r,0)).toString().append(model->data(model->index(r,1)).toString().prepend(" ")) );

        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        item->setData(Qt::Unchecked, Qt::CheckStateRole);

        model1->setItem(r, 0, item);

        qDebug()<< model1->data(model1->index(r,0)).toString();
    }


    ui->Checkablewidget->setModel(model1);

    tmp_incident->set_i_Agence(ui->comboBox_Agence->itemText(index));
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));
}

void Form_incident::on_tableView_presence_doubleClicked(const QModelIndex &index)
{
    if(!tmp_incident->getI_Presence().isEmpty()){
        QString str=ui->tableView_presence->model()->data(ui->tableView_presence->model()->index(index.row(),0) ).toString();
        //
        for(int i=0;i<tmp_incident->getI_Presence().size();i++){

            if(tmp_incident->getI_Presence().at(i).split("#")[0].contains(str))
            {
                tmp_incident->remove_Presence(i);

            }

        }
        ui->tableView_presence->model()->removeRow(index.row());
    }
    QSoundEffect *sound = new QSoundEffect;
    sound->setSource(QUrl("qrc:/files/data/bin.wav"));
    sound->play();
}

void Form_incident::on_pushButton_clicked()
{
    tmp_incident->setI_notes(((QStringListModel*)ui->listView->model())->stringList());
    EmployerModel *model=new EmployerModel(this,manager->getDatabase());
    model->setTable("INCIDENTS");
    model->select();

    model->insertRow(0);
    model->setData(model->index(0, 1),tmp_incident->get_i_Objet() );
    model->setData(model->index(0, 2),tmp_incident->getI_Direction());

    model->setData(model->index(0, 3),tmp_incident->get_i_Agence() );
    model->setData(model->index(0, 4),tmp_incident->get_i_Date() );
    model->setData(model->index(0, 5),tmp_incident->getI_Date_Cmd() );
    model->setData(model->index(0, 6),tmp_incident->getI_Date_Facture() );
    model->setData(model->index(0, 7),tmp_incident->get_i_Heure_debut_fin().join("\n<--->\n")  );
    model->setData(model->index(0,8),tmp_incident->get_i_Commande() );
    model->setData(model->index(0, 9), tmp_incident->get_i_Convention() );

    model->setData(model->index(0, 10),tmp_incident->getI_Travaux()->getAll().join("\n<--->\n"));

    model->setData(model->index(0, 11),tmp_incident->getI_Presence().join("\n<--->\n")  );
    model->setData(model->index(0, 12), tmp_incident->get_i_Agent().join("\n") );
    model->setData(model->index(0, 13),tmp_incident->get_i_Montant() );
    model->setData(model->index(0, 14),tmp_incident->getI_notes().join("\n#########\n") );

    model->setData(model->index(0, 15),F_form_type==Form_incident::INCIDENT ? "Incident":"Entretien" );




    tmp_incident->i_print();
    QApplication::beep();
    //tmp_fct();

    //emit wn->findChild<QTabWidget*>("tabWidget")->windowTitleChanged("zeta");
}
void Form_incident::tmp_fct(){

    QSqlTableModel *modeltmp= new QSqlTableModel(this,manager->getDatabase());
    QSqlTableModel *model = new QSqlTableModel(this,manager->getDatabase());
    modeltmp->setTable("Actes");
    foreach (QString table, manager->getDbTables()) {
        if(!(table.contains("Agents" )||table.contains("Agence" )||table.contains("INCIDENTS" )||table.contains("Ressources_Humaines" )||table.contains("Actes" )||table.contains("sqlite" ))) {
            QString tablename(table);
            tablename.push_front("\"");
            tablename.push_back("\"");
            model->setTable(tablename);
            model->select();
            for (int i = 0; i < model->rowCount(); ++i) {
                modeltmp->select();
                modeltmp->insertRow(0);
                modeltmp->setData(modeltmp->index(0,0),model->data(model->index(i,0),Qt::DisplayRole).toString(),Qt::EditRole);
                modeltmp->setData(modeltmp->index(0,1),model->data(model->index(i,1),Qt::DisplayRole).toString(),Qt::EditRole);
                modeltmp->setData(modeltmp->index(0,2),model->data(model->index(i,2),Qt::DisplayRole).toDouble(),Qt::EditRole);
                modeltmp->setData(modeltmp->index(0,3),model->data(model->index(i,3),Qt::DisplayRole).toDouble(),Qt::EditRole);
                table.remove(QChar('\''));
                table.remove(QChar('"'));
                modeltmp->setData(modeltmp->index(0,4),table,Qt::EditRole);
                modeltmp->data(modeltmp->index(i,0),Qt::DisplayRole).toString();
                modeltmp->submit();
            }
        }
    }


}
void Form_incident::on_comboBox_Agent_editTextChanged(const QString &arg1)
{
    //tmp_incident->set_i_Agent(arg1);
}

void Form_incident::on_dateEdit_2_editingFinished()
{
    tmp_incident->setI_Date_Cmd(ui->dateEdit_2->date().toString("dd/MM/yyyy"));
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));

}

void Form_incident::on_timeEdit_Debut_editingFinished()
{
    QSettings settings("Redanium", "Oso Manager");
    QString debut=settings.value("general/travail_nuit_debut").toString();
    QString fin=settings.value("general/travail_nuit_fin").toString();
    tmp_incident->set_i_Heure_debut_fin(ui->timeEdit_Debut->time().toString("hh:mm"),0);

    if( tmp_incident->get_i_Heure_debut_fin().at(0).split(":").at(0).toInt() >= debut.split(":").at(0).toInt()  )
    {tmp_incident->setMajoration_Fifty_Night(true);
        ui->label_6->setPixmap(QPixmap(":/files/images/green.png"));}
    else    {
        tmp_incident->setMajoration_Fifty_Night(false);
        ui->label_6->setPixmap(QPixmap(":/files/images/red.png"));
    }
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));


}

void Form_incident::on_timeEdit_Fin_editingFinished()
{
    QSettings settings("Redanium", "Oso Manager");
    QString debut=settings.value("general/travail_nuit_debut").toString();
    QString fin=settings.value("general/travail_nuit_fin").toString();

    tmp_incident->set_i_Heure_debut_fin(ui->timeEdit_Fin->time().toString("hh:mm"),1);
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));

}

void Form_incident::on_toolButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()<ui->stackedWidget->count()-1?ui->stackedWidget->currentIndex()+1:0);
}

void Form_incident::on_toolButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex()>0?ui->stackedWidget->currentIndex()-1:ui->stackedWidget->count()-1);

}

void Form_incident::on_spinBox_Cmd_Num_EditingFinished()
{
    tmp_incident->set_i_Commande(ui->spinBox_Cmd_Num->value());
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));



}

void Form_incident::hideHbox(QHBoxLayout *layout)
{
    //    foreach (QObject var, layout->children()) {


    //    }

}

void Form_incident::updateNotes()
{
    tmp_incident->setI_notes( ((QStringListModel*)ui->listView->model())->stringList());

}

void Form_incident::on_comboBox_2_currentIndexChanged(int index)
{
    switch (index) {
    case 1:
        tmp_incident->setImputation(Incident_obj::AERIEN);
        if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
        else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
        else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));

        break;
    case 2:
        tmp_incident->setImputation(Incident_obj::SOUSTERRAIN);
        if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
        else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
        else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));


        break;
    case 3:
        tmp_incident->setImputation(Incident_obj::POSTE);
        if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
        else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
        else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));

        break;
    default:
        break;
    }
}



void Form_incident::on_toolButton_3_clicked()
{

}

void Form_incident::on_lineEdit_Objet2_textChanged(QString arg)
{
    //    {
    //        QString strObj ;
    //        strObj.append(" ");
    //        strObj.append(ui->comboBox->currentText());
    //        strObj.append(" ");
    //        strObj.append(ui->lineEdit_Objet2->text());
    //        tmp_incident->setI_address(strObj);
    //    }
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));
}

void Form_incident::on_textEdit_textChanged()
{

    tmp_incident->set_i_Objet(ui->plainTextEdit->document()->toPlainText());
    ui->plainTextEdit->setStyleSheet(ui->plainTextEdit->document()->toPlainText().isEmpty()?  " QPlainTextEdit#plainTextEdit { background-color: lightyellow }" : "QPlainTextEdit#plainTextEdit { background-color: white }" );
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));

}

void Form_incident::on_toolButton_5_clicked()
{
    ui->listView->model()->insertRow(0);

}

void Form_incident::on_comboBox_3_currentIndexChanged(const QString &arg1)
{
    int index;
    QSqlTableModel *model = new QSqlTableModel(this,manager->getDatabase());

    model->setTable("Agence");
    if (ui->comboBox_3->currentText().contains("Oran") ){index=1; tmp_incident->setI_Direction("Oran");}
    if (ui->comboBox_3->currentText().contains("Es-Senia")) {index=2;tmp_incident->setI_Direction("Es-Senia");}

    model->setFilter(QString("Direction =\'"+QString::number(index)+"\'"));
    model->select();
    ui->comboBox_Agence->setModel(model);
    if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==0) tmp_incident->i_Preview(0,i_MainWindow->findChild<QWebView*>("webView_2"));
    else if(i_MainWindow->findChild<QTabWidget*>("tabWidget_3")->currentIndex()==1) tmp_incident->i_Preview(1,i_MainWindow->findChild<QWebView*>("webView_2"));
    else  tmp_incident->i_Preview(2,i_MainWindow->findChild<QWebView*>("webView_2"));
}
int Form_incident::getF_form_type() const
{
    return F_form_type;
}

void Form_incident::setF_form_type(int value)
{
    F_form_type = value;
}

