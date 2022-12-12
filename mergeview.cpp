#include "mergeview.h"
#include <QDebug>
#include <QHash>
#include <QProcess>
#include <QSettings>
#include "filetreemodel.h"

QString getPair(QString extension, QString app) {
    QString sep = QString(":");
    return extension + sep + app;
}

QHash<QString, QString> runnersListToRunners(QList<QString> runnersList) {
    QHash<QString, QString> hash = QHash<QString, QString>();
    foreach (QString pair, runnersList) {
        QStringList lst = pair.split(QRegExp(":"));
        hash.insert(lst.at(0), lst.at(1));
    }
    return hash;
}

MergeView::MergeView(QWidget *parent) : QTreeView(parent)
{
    QList<QString> defaultFolders = QList<QString>();
    defaultFolders.append("~");
    QStringList keys = settings.allKeys();
    if (!keys.contains("folders")) {
        settings.setValue("folders", QVariant(defaultFolders));
    }
    QList<QString> folders = settings.value("folders").toStringList();

    // SET RUNNERS
    QList<QString> defaultRunners = QList<QString>();
    defaultRunners.append(getPair("[.]txt$", "emacs"));
    defaultRunners.append(getPair("[.]html$", "emacs"));
    defaultRunners.append(getPair("[.]rb$", "emacs"));
    defaultRunners.append(getPair("[.]py$", "emacs"));
    defaultRunners.append(getPair("[.]cpp$", "emacs"));
    defaultRunners.append(getPair("[.]hs$", "emacs"));
    defaultRunners.append(getPair("[.]tex$", "emacs"));
    defaultRunners.append(getPair("[.]png$", "phototonic"));
    defaultRunners.append(getPair("[.]svg$", "inkscape"));
    defaultRunners.append(getPair("[.]ipe$", "ipe"));
    defaultRunners.append(getPair("[.]pdf$", "evince"));
    defaultRunners.append(getPair("[.]ps$", "evince"));
    defaultRunners.append(getPair("[.]eps$", "evince"));
    defaultRunners.append(getPair("[.]docx$", "libreoffice"));
    if (!keys.contains("runners")) {
        settings.setValue("runners", QVariant(defaultRunners));
    }
    runners = runnersListToRunners(settings.value("runners").toStringList());


    fileTreeModel = new FileTreeModel(folders, this);
    setModel(fileTreeModel);
    setColumnWidth(0, 500);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClicked(QModelIndex)));
}

void MergeView::changeFolders(QList<QString> &folders) {
    fileTreeModel = new FileTreeModel(folders, this);
    setModel(fileTreeModel);
}

QHash<QString, QString> MergeView::getRunners() const
{
    return runners;
}



void MergeView::runFile(const QString filepath) {
    QList<QString> keys = runners.keys();

    bool run = false;
    for (int i = 0; i < keys.length(); i++) {
        // QString extension = keys.at(i);
        // if (filepath.endsWith(extension)) {
        QRegExp fileRegexp = QRegExp(keys.at(i));
        if (fileRegexp.indexIn(filepath) > -1){
            // QProcess::execute(runners.value(extension), {filepath});
            QProcess::execute(runners.value(keys.at(i)), {filepath});
            run = true;
            break;
        }
    }
    // if (!run) {
    //    QProcess::execute("emacs", {filepath});
    // }
}

void MergeView::doubleClicked(const QModelIndex &index) {
    QString filepath = fileTreeModel->data(index.siblingAtColumn(2), Qt::DisplayRole).toString();
    if (filepath.length() > 0) {
        runFile(filepath);
    }
}

void MergeView::keyPressEvent(QKeyEvent *event)
{
    bool filepathRun = false;
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Right) {
        QModelIndexList list = this->selectedIndexes();
        if (list.length() > 0) {
            QModelIndex relevantIndex = list.at(0).siblingAtColumn(2);
            QString filepath = fileTreeModel->data(relevantIndex, Qt::DisplayRole).toString();
            if (filepath.length() > 0) {
                runFile(filepath);
                filepathRun = true;
            }
        }
    }
    if (!(filepathRun && event->key() == Qt::Key_Right)) {
        QTreeView::keyPressEvent(event);
    }
}
