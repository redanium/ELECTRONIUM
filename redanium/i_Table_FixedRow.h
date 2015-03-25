#ifndef I_TABLE_FIXEDROW_H
#define I_TABLE_FIXEDROW_H

#include <QWidget>
#include"incident.h"


namespace Ui {
class i_table_fixedrow;
}

class i_table_fixedrow : public QWidget
{
    Q_OBJECT

public:
    explicit i_table_fixedrow(QWidget *parent = 0);
    ~i_table_fixedrow();

    Incident_obj *getIncident() const;
    void setIncident(Incident_obj *value);
    void updateTotal(int table);


private slots:


    void on_label_2_linkActivated(const QString &link);

    void on_toolButton_Meo_clicked();

    void on_toolButtonFournitures_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

private:
    Ui::i_table_fixedrow *ui;
    Incident_obj  *incident;

};

#endif // I_TABLE_FIXEDROW_H
