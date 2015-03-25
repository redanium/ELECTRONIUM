#ifndef CHECKABLELISTWIDGET_H
#define CHECKABLELISTWIDGET_H

#include <QWidget>
#include<QtWidgets>

namespace Ui {
class checkableListWidget;
}

class checkableListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit checkableListWidget(QWidget *parent = 0);
    ~checkableListWidget();

    void setModel(QStandardItemModel *model);
    QStandardItemModel *model();

private slots:
    void on_toolButton_clicked();
    void insertItem(bool checked);
    void on_toolButton__clicked();

    void addItem(QString itemText);
signals:
    void addingItem(QString text);
    void deletingItem(QObject* obj);
private:
    Ui::checkableListWidget *ui;
    QMenu  *m_menu;
    QStandardItemModel *m_model;
    QSignalMapper *signalMapper;

};

#endif // CHECKABLELISTWIDGET_H
