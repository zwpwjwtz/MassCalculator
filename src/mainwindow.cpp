#include <QKeyEvent>
#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);

    ui->tabMass->loadPreferences();
    ui->tabFormula->loadPreferences();
}

MainWindow::~MainWindow()
{
    ui->tabMass->savePreferences();
    ui->tabFormula->savePreferences();
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

bool MainWindow::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::KeyPress)
    {
        keyPressEvent(static_cast<QKeyEvent*>(event));
        return event->isAccepted();
    }
    return QObject::eventFilter(object, event);
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    if (e->modifiers() == Qt::KeyboardModifier::AltModifier)
    {
        switch (e->key())
        {
            case Qt::Key::Key_1:
                ui->tabWidget->setCurrentIndex(0);
                break;
            case Qt::Key::Key_2:
                ui->tabWidget->setCurrentIndex(1);
                break;
            case Qt::Key::Key_3:
                ui->tabWidget->setCurrentIndex(2);
                break;
            default:;
        }
        e->accept();
    }
    else
        e->ignore();
}
