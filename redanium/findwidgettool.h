#ifndef FINDWIDGETTOOL_H
#define FINDWIDGETTOOL_H

#include <QWidget>

namespace Ui {
class FindWidgetTool;
}

class FindWidgetTool : public QWidget
{
    Q_OBJECT

public:
    explicit FindWidgetTool(QWidget *parent = 0);
    ~FindWidgetTool();

private slots:
    void on_lineEditTravaux_editingFinished();

    void on_toolButton_clicked();

    void on_toolButton_switch_clicked();

private:
    Ui::FindWidgetTool *ui;
};

#endif // FINDWIDGETTOOL_H
