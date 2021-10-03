#include <QApplication>
#include <QTranslator>
#include "global.h"
#include "config.h"
#include "mainwindow.h"


Config appConfig;
QTranslator appTranslator;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    appTranslator.load(QString(":/translation/MassCalculator_")
                       .append(QLocale::system().name()));
    a.installTranslator(&appTranslator);

    MainWindow w;
    w.show();

    return a.exec();
}
