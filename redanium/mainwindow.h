#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"DatabaseManager.h"
#include"incident.h"
#include"configdialog.h"
#include"Form_Incident.h"
#include"Incidents_Form.h"
#include"faderwidget.h"
#include<QtWebKitWidgets>
#include<QPrinter>




namespace Ui {
class MainWindow;
}
class Form_incident;
class MainWindow : public QMainWindow

{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    static int FindItemRow(QString item,QTableView *table);

    void UpdateMoney();

    inline DatabaseManager* getManager(){return manager;}

    int Print(QPrinter *printer ,QPrinter::OutputFormat outputFormat =QPrinter::NativeFormat ,QString filename="",bool isPreview =false);

public slots:
    void updateQty();
private slots:
void checkData();
    void filePrintPreview();

    void filePrintPDF();

    int Printp(QPrinter *printer );

    void showIncidentsTable();

    void draw_graph_tab();

    void saveOptions();

    void readOptions();

    void on_tableView_Travaux_doubleClicked(const QModelIndex &index);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_actionOptions_triggered();

    void on_actionSur_base_de_donn_es_triggered();

    void on_actionSur_disque_triggered();

    void on_actionIncident_triggered();

    void on_actionImprimer_2_triggered();

    void on_actionSauvegarder_triggered();

    void on_actionOuvrir_triggered();

    void on_mdiArea_subWindowActivated(QMdiSubWindow *arg1);

    void on_actionCascade_triggered();

    void on_actionMinimiser_triggered();

    void on_actionMaximiser_triggered();

    void on_actionSuivant_triggered();

    void on_actionPr_cedent_triggered();

    void on_actionFermer_triggered();

    void on_actionFermer_les_fen_tres_triggered();

    void on_toolButtonSearch_clicked();

    void on_action_lock_trigered();

    void on_lineEditTravaux_editingFinished();

    void on_actionImprimer_Pdf_triggered();

    void on_pushButton_clicked();

    void on_PdfBtn_clicked();

    void on_toolButton_clicked();

    void on_tabWidget_3_currentChanged(int index);

    void on_tabWidget_3_tabBarClicked(int index);

    void on_actionSwitch_triggered();

    void on_toolButton_4_clicked();

    void handleResponse(QNetworkReply* reply);


    void updateMarker(QStringList args);

    void updateLocationtable(QStringList args);

    void clean(QObject* obj);

    void on_toolButton_6_clicked();

    void on_toolButton_7_clicked();

    void on_listView_clicked(const QModelIndex &index);

    void on_actionEntretien_triggered();

    void activateSubWindow(QWidget* widget);

    void on_actionSwitch2_triggered();

    void on_actionApercu_triggered();

private:

    configDialog *dialog;// dialog for settings
    Ui::MainWindow *ui;
    DatabaseManager *manager; //Db manager
    QSettings m_Settings ;
    static Incidents_Form *Incidents_Form_List;//must create only one instance+update_Ui fcts
    Form_incident *currentActiveForm;  //
    QWebView *webview;
    QPointer<FaderWidget> faderWidget;

    QMap<QString,QStandardItemModel*> gpsDevicesModels;
    QMap<QString,Form_incident*> incidents;
    QMap<QString,Form_incident*> entretiens;
    QSignalMapper *windowMapper;
    QList<QToolBar*> toolbars;

};

#endif // MAINWINDOW_H








