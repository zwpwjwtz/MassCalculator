#include "QMessageBox"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calculator/atomname.h"
#include "calculator/formulagenerator.h"
#include "widget/compositionselector.h"
#include "widget/frameformulalist.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->textInputFormula->setFocus();

    compositionList = new CompositionSelector(ui->tabFormula);
    compositionList->setAutoFillBackground(true);
    compositionList->hide();
    showAllowedElementRanges();
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

    ui->labelVersion->setText(QString(tr("Ver: %1")).arg(APP_VERSION));
    ui->textAbout->setPlainText(
                tr("MassCalculator - A simple tool for calculating molecular weight.\n\n"
                   "This program is a free software.\n\n"
                   "You can redistribute it and/or modify it under the terms of "
                   "the GNU Library General Public License as published by "
                   "the Free Software Foundation; either version 3 of the License, "
                   "or (at your option) any later version.\n\n"
                   "This program is distributed in the hope that it will be useful, but WITHOUT "
                   "ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or "
                   "FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License "
                   "for more details.\n"));
}

void MainWindow::showAllowedElementRanges()
{
    QString displayedRanges;
    auto rangeList = compositionList->getElementRanges();
    for (auto i=rangeList.cbegin(); i!=rangeList.cend(); i++)
    {
        displayedRanges.append(
            QString("%1%2-%3 ").arg(QString::fromStdString(
                                       AtomName::abbreviation(i->atomNumber)),
                                   QString::number(i->minCount),
                                   QString::number(i->maxCount)));
    }
    ui->textAllowedElement->setText(displayedRanges);
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
    on_buttonGetMass_clicked();
}

void MainWindow::on_textInputMass_returnPressed()
{
    on_buttonGetFormula_clicked();
}

void MainWindow::on_buttonGetMass_clicked()
{
    Formula f;
    bool ok = f.parse(ui->textInputFormula->text().toStdString());
    if (!ok)
    {
        QMessageBox::warning(this, "Invalid formula",
                             "The formula that you input is incorrect.");
        return;
    }
    ui->textResultMass->setText(QString::number(f.toAverageMass()));
    ui->textResultMonoMass->setText(QString::number(f.toMass(), 'f', 6));
    ui->textResultFormula->setText(QString::fromStdString(f.toString()));
}

void MainWindow::on_buttonGetFormula_clicked()
{
    FormulaGenerator generator;

    double mass = ui->textInputMass->text().toDouble();
    double toleranceMin = ui->textMassToleranceLeft->text().toDouble();
    double toleranceMax = ui->textMassToleranceRight->text().toDouble();
    if (ui->radioMassToleranceRelative->isChecked())
    {
        toleranceMin *= mass * 1E-6;
        toleranceMax *= mass * 1E-6;
    }

    auto elementRanges = compositionList->getElementRanges();
    for (auto i=elementRanges.cbegin(); i!=elementRanges.cend(); i++)
        generator.setElement(*i);

    auto result = generator.fromMass(mass + toleranceMin, mass + toleranceMax);
    if (result.size() < 1)
        ui->statusBar->showMessage("No result found.");
    else
    {
        ui->statusBar->showMessage(QString("%1 result(s) found.")
                                          .arg(result.size()));
        ui->frameResultFormula->loadResult(result, mass);
    }
}

void MainWindow::on_buttonAllowedElement_clicked()
{
    if (compositionList->isVisible())
    {
        compositionList->hide();
        showAllowedElementRanges();
    }
    else
    {
        compositionList->move({ui->textAllowedElement->x(),
                               ui->textAllowedElement->y() +
                               ui->textAllowedElement->height()});
        compositionList->resize(ui->textAllowedElement->width(),
                                compositionList->height());
        compositionList->show();
    }
}
