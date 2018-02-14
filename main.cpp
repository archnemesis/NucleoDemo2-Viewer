#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("RobinGingras");
    QCoreApplication::setOrganizationDomain("robingingras.com");
    QCoreApplication::setApplicationName("NucleoDemo2-Viewer");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
