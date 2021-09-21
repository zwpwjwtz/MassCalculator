#include "QMessageBox"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "calculator/formulagenerator.h"
#include "widget/compositionselector.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->listResultFormula->setModel(&formulaList);
    ui->textInputFormula->setFocus();

    formulaList.setColumnCount(4);
    formulaList.setHorizontalHeaderLabels({"Formula",
                                           "Mono.Mass",
                                           "Difference(Da)",
                                           "Difference(ppm)"});
    ui->listResultFormula->horizontalHeader()
                         ->setSectionResizeMode(QHeaderView::Stretch);

    compositionList = new CompositionSelector(ui->tabFormula);
    compositionList->setAutoFillBackground(true);
    compositionList->hide();
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
            ui->listResultFormula->horizontalHeader()
                            ->setSectionResizeMode(QHeaderView::Interactive);
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
    ui->textResultMonoMass->setText(QString::number(f.toMass()));
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
    formulaList.setRowCount(0);
    if (result.size() < 1)
        ui->statusBar->showMessage("No result found.");
    else
    {
        // Load results
        double formulaMass;
        QList<QStandardItem*> newRow;
        for (auto j=result.cbegin(); j!=result.cend(); j++)
        {
            formulaMass = j->toMass();
            newRow << new QStandardItem(QString::fromStdString(j->toString()))
                   << new QStandardItem(QString::number(formulaMass))
                   << new QStandardItem(QString::number(mass - formulaMass))
                   << new QStandardItem(
                          QString::number((mass - formulaMass) / mass  * 1E6));
            formulaList.appendRow(newRow);
            newRow.clear();
        }
        ui->statusBar->showMessage(QString("%1 result(s) found.")
                                          .arg(result.size()));
    }
}

void MainWindow::on_buttonAllowedElement_clicked()
{
    if (compositionList->isVisible())
        compositionList->hide();
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
