#include <QApplication>
#include "global.h"
#include "config.h"
#include "mainwindow.h"


Config appConfig;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
