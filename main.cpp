#include "mainwindow.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationName("foldermergeview");
    QCoreApplication::setOrganizationName("ahmet.ac");
    QCoreApplication::setOrganizationDomain("ahmet.ac");

    MainWindow w;
    w.show();

    QStringList arguments = a.arguments();
    if (arguments.length() > 1) {
        QStringList folders = QStringList();
        for (int i = 1; i < arguments.length(); i++) {
            folders.append(arguments.at(i));
        }
        w.getMergeView()->changeFolders(folders);
    }
    return a.exec();
}
