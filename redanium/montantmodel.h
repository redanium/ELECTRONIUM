#ifndef MONTANTMODEL_H
#define MONTANTMODEL_H

#include <QSqlTableModel>

class montantModel : public QSqlTableModel
{
    Q_OBJECT
public:
    montantModel();

    // QAbstractItemModel interface
public:
    bool setData(const QModelIndex &index, const QVariant &value, int role);

};

#endif // MONTANTMODEL_H
