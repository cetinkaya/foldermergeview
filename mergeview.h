#ifndef MERGEVIEW_H
#define MERGEVIEW_H

#include <QTreeView>
#include <QObject>
#include <QWidget>
#include <QList>
#include <QDir>
#include <QSettings>
#include <QKeyEvent>

class FileTreeModel;

class MergeView : public QTreeView
{
    Q_OBJECT
public:
    MergeView(QWidget *parent);
    void runFile(const QString filepath);
    QHash<QString, QString> getRunners() const;

public slots:
    void doubleClicked(const QModelIndex &index);
    void changeFolders(QList<QString> &folders);
private:
    FileTreeModel *fileTreeModel;
    QList<QDir> folders;
    QHash<QString, QString> runners;
    QSettings settings;
protected:
     void keyPressEvent(QKeyEvent *event);
};

#endif // MERGEVIEW_H
