#include "QMessageBox"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calculator/formulamass.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textInputFormula->setFocus();
}

MainWindow::~MainWindow()
{
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

void MainWindow::on_tabWidget_currentChanged(int index)
{
    switch (index)
    {
        case 0: // Formula to mass
            ui->textInputFormula->setFocus();
            break;
        case 1: // Mass to formula
            ui->textInputMass->setFocus();
            break;
        default:;
    }
}

void MainWindow::on_textInputFormula_returnPressed()
{
    bool error;
    double mass = FormulaMass::toMass(
                    ui->textInputFormula->text().toLocal8Bit().toStdString(),
                    &error);
    if (error)
        QMessageBox::warning(this, "Invalid formula",
                             "The formula that you input is incorrect.");
    else
        ui->textResultMonoMass->setText(QString::number(mass));
}
