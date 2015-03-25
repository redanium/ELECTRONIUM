#ifndef INCIDENTSFORM_H
#define INCIDENTSFORM_H

#include <QWidget>
#include<QMenu>

namespace Ui {
class Incidents_Form;
}

class Incidents_Form : public QWidget
{
    Q_OBJECT

public:
    enum f_TYPE {INCIDENT=0,ENTRETIEN=1}; //

    explicit Incidents_Form(QWidget *parent = 0);
    ~Incidents_Form();
    void updatePlot();

public slots:


    void connectMenu(int i);



private slots:


    void on_tableView_clicked(const QModelIndex &index);

    void on_comboBox_2_currentIndexChanged(const QString &arg1);

    void on_comboBox_currentIndexChanged(const QString &arg1);

    void on_lineEdit_editingFinished();

    void on_toolButton_3_clicked();

    void on_toolButton_4_clicked();

    void on_toolButton_5_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_7_clicked();

    void on_toolButton_6_clicked();
    
    void on_toolButton_8_clicked();

    void on_toolButton_9_clicked();

    void on_toolButton_open_clicked();

private:
    int F_form_type;
    QDialog *colDlg;
    Ui::Incidents_Form *ui;
};

#endif // INCIDENTSFORM_H
