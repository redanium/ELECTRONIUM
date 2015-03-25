#include "findwidgettool.h"
#include "ui_findwidgettool.h"
#include "Form_Incident.h"

FindWidgetTool::FindWidgetTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FindWidgetTool)
{
    ui->setupUi(this);
}

FindWidgetTool::~FindWidgetTool()
{
    delete ui;
}

void FindWidgetTool::on_lineEditTravaux_editingFinished()
{
    MainWindow *M_wn = (MainWindow*) parent()->parent();
    QString str;
      str.append(ui->comboBox_toolbar->currentText());
      if(ui->comboBox_3->currentText().contains("contient"))
      {
          str.append(" LIKE ");
          str.append(" '%");
          str.append(ui->lineEditTravaux->text());
          str.append("%' ");
          ((EmployerModel*)M_wn->findChild<QTableView*>("tableView_Travaux")->model())->setFilter(str);

      }
      else {
          str.append(ui->comboBox_3->currentText());
          str.append("'");
          str.append(ui->lineEditTravaux->text());
          str.append("'");
          ((EmployerModel*)M_wn->findChild<QTableView*>("tableView_Travaux")->model())->setFilter(str);
      }
      ui->lineEditTravaux->setStyleSheet(ui->lineEditTravaux->text().isEmpty()? "QLineEdit#lineEditTravaux { background-color: lightyellow }":"QLineEdit#lineEditTravaux { background-color: white }");
      ui->label->setText(QString::number(M_wn->findChild<QTableView*>("tableView_Travaux")->model()->rowCount()));
}

void FindWidgetTool::on_toolButton_clicked()
{
}

void FindWidgetTool::on_toolButton_switch_clicked()
{
    ui->stackedWidgetFindTravaux->setCurrentIndex(ui->stackedWidgetFindTravaux->currentIndex()==0?1:0);

}
