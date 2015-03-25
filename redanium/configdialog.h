#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>

namespace Ui {
class configDialog;
}

class configDialog : public QDialog
{
    Q_OBJECT

public:
    explicit configDialog(QWidget *parent = 0);
    ~configDialog();

private slots:
    void on_listView_clicked(const QModelIndex &index);

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_toolButton_3_clicked();

    void on_toolButton_4_clicked();

    void on_toolButton_5_clicked();

    void on_toolButton_6_clicked();

    void on_toolButton_8_clicked();

    void on_toolButton_7_clicked();

    void on_toolButton_10_clicked();

    void on_toolButton_9_clicked();

private:
    Ui::configDialog *ui;
};

#endif // CONFIGDIALOG_H
