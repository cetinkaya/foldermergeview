#include "filetreemodel.h"
#include <QHash>
#include <QDebug>
#include <QFile>
#include <QDateTime>
#include <QDate>
#include <QtAlgorithms>

FileTreeModel::FileTreeModel(const QList<QString> &folders, QObject *parent)
{
    rootItem = new Item({"File", "Date", "Filepath"}, NULL);
    mergeView = (MergeView*)parent;
    addData(folders);
}

QModelIndex FileTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    Item *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<Item*>(parent.internalPointer());

    Item *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex FileTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    Item *childItem = static_cast<Item*>(index.internalPointer());
    Item *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int FileTreeModel::rowCount(const QModelIndex &parent) const
{
    Item *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<Item*>(parent.internalPointer());

    return parentItem->childCount();
}

int FileTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<Item*>(parent.internalPointer())->columnCount();
    return rootItem->columnCount();
}

QVariant FileTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DecorationRole && index.column() == 0) {
        Item *item = static_cast<Item*>(index.internalPointer());
        return item->icon();
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    Item *item = static_cast<Item*>(index.internalPointer());

    return item->data(index.column());
}

QVariant FileTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

Qt::ItemFlags FileTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QString date(QString path) {
    QFileInfo info = QFileInfo(path);
    return info.lastModified().date().toString("MMM d, yyyy");
}

bool less1(int a, int b) {
    return b < a;
}

bool lessDate(QString a, QString b) {
    QDateTime dateA = QFileInfo(a).lastModified();
    QDateTime dateB = QFileInfo(b).lastModified();
    return dateB < dateA;
}

void FileTreeModel::addData(const QList<QString> &folders)
{
    QList<QString> allFiles = QList<QString>();
    for(int i = 0; i < folders.length(); i++) {
        QFileInfo info = QFileInfo(folders.at(i));
        if(info.exists() && info.isDir()) {
            QDir folder = QDir(folders.at(i));
            QList<QString> *files = getFiles(folder);
            for(int j = 0; j < files->length(); j++) {
                allFiles.append(files->at(j));
            }
            free(files);
        }
    }
    QHash<int, QStringList*> hash;
    for(int i = 0; i < allFiles.length(); i++) {
        QFileInfo info = QFileInfo(allFiles.at(i));
        int year = info.lastModified().date().year();

        if (hash.contains(year)) {
            QStringList *list = hash.value(year);
            list->append(allFiles.at(i));
        } else {
            QStringList *list = new QStringList();
            list->append(allFiles.at(i));
            hash.insert(year, list);
        }
    }

    QList<int> years = hash.keys();
    std::sort(years.begin(), years.end(), less1);
    for (int i = 0; i < years.length(); i++) {
        int year = years.at(i);
        Item *yearItem = new Item({QString::number(year), "", ""}, rootItem);
        rootItem->appendChild(yearItem);
        QStringList *list = hash.value(year);
        std::sort(list->begin(), list->end(), lessDate);
        for (int j = 0; j < list->length(); j++) {
            QFileInfo info = QFileInfo(list->at(j));
            QString filename = info.fileName();
            if (filename.contains(".")) {
                Item *fileItem = new Item({QString(info.fileName()), date(list->at(j)), list->at(j)}, yearItem);
                yearItem->appendChild(fileItem);
            }
        }
    }
}

bool noExpand(QString path) {
    QList<QString> strs = {"vendor/bundle"};
    for (int i = 0; i < strs.length(); i++) {
        if (path.contains(strs.at(i))) {
            return true;
        }
    }
    return false;
}

bool matchesOne(QList<QString> lst, QString value) {
    foreach (QString regexStr, lst) {
        QRegExp regex = QRegExp(regexStr);
        if (regex.indexIn(value) > -1) {
            return true;
        }
    }
    return false;
}

QList<QString> *FileTreeModel::getFiles(QDir folder)
{
    QList<QString> *allFiles = new QList<QString>();
    QStringList list = folder.entryList();
    for(int i = 0; i < list.length(); i++) {
        QString value = list.at(i);
        if (!value.startsWith(".")) {
            QString absoluteValue = folder.absoluteFilePath(value);
            QFileInfo info = QFileInfo(absoluteValue);
            if (info.exists()) {
                if (info.isDir()) {
                    if (!noExpand(absoluteValue)){
                        QList<QString> *files = getFiles(QDir(absoluteValue));
                        for(int j = 0; j < files->length(); j++) {
                            allFiles->append(files->at(j));
                        }
                        free(files);
                    }
                } else {
                    if (matchesOne(mergeView->getRunners().keys(), absoluteValue)) {
                        allFiles->append(absoluteValue);
                    }
                }
            }
        }
    }
    return allFiles;
}

