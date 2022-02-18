#include <QApplication>
#include <QTranslator>
#include "global.h"
#include "component/config.h"
#include "mainwindow.h"


Config appConfig;
QTranslator appTranslator;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.installTranslator(&appTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
