#ifndef FILETREEMODEL_H
#define FILETREEMODEL_H

#include <QAbstractItemModel>
#include <QObject>
#include <QWidget>
#include <QList>
#include <QFile>
#include <QDir>
#include <QHash>
#include "item.h"
#include "mergeview.h"

class FileTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    FileTreeModel(const QList<QString> &folders, QObject *parent);

    // QAbstractItemModel interface
public:
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    Item *rootItem;
    void addData(const QList<QString> &folders);
    QList<QString> *getFiles(QDir folder);
    MergeView *mergeView;
};

#endif // FILETREEMODEL_H
