#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "global.h"
#include "config.h"
#include "calculator/atomname.h"
#include "calculator/atommass.h"
#include "thread/formulageneratorworker.h"
#include "thread/formulageneratorbatchworker.h"
#include "widget/compositionselector.h"
#include "widget/frameformulalist.h"

#define MC_TAB_INDEX_GETMASS       0
#define MC_TAB_INDEX_GETFORMULA    1
#define MC_TAB_INDEX_ABOUT         2

#define MC_FORMULA_ELEMENT_ORDER   {"C"}

#define MC_FORMULA_FILE_SUFFIX_ALL "All (*)"
#define MC_FORMULA_FILE_SUFFIX_CSV "CSV (*.csv)"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->frameModification->hide();
    ui->frameMassModification->hide();
    ui->frameImportMassFromFile->hide();
    ui->textInputMass->installEventFilter(this);
    ui->textInputMass->setLabelTextFormat(Qt::RichText);
    ui->textInputFormula->installEventFilter(this);
    ui->textInputFormula->setLabelTextFormat(Qt::RichText);
    ui->textMassToleranceLeft->installEventFilter(this);
    ui->textMassToleranceRight->installEventFilter(this);
    connect(ui->frameModification, SIGNAL(chargeChanged()),
            this, SLOT(onFormulaModificationChanged()));
    connect(ui->frameModification, SIGNAL(modificationChanged()),
            this, SLOT(onFormulaModificationChanged()));
    connect(ui->frameMassModification, SIGNAL(chargeChanged()),
            this, SLOT(onMassModificationChanged()));
    connect(ui->frameMassModification, SIGNAL(modificationChanged()),
            this, SLOT(onMassModificationChanged()));

    labelFileLink = new QLabel(this);
    labelFileLink->setVisible(false);
    labelFileLink->setTextFormat(Qt::TextFormat::RichText);
    labelFileLink->setText("Open the <a href=\"#\">result</a>&nbsp;&nbsp;");
    ui->statusBar->addPermanentWidget(labelFileLink);
    connect(labelFileLink, SIGNAL(linkActivated(const QString&)),
            this, SLOT(onLabelFileLinkClicked()));

    progressBar = new QProgressBar(this);
    progressBar->setValue(0);
    progressBar->setVisible(false);
    ui->statusBar->addPermanentWidget(progressBar);

    compositionList = new CompositionSelector(ui->tabFormula);
    compositionList->setAutoFillBackground(true);
    compositionList->hide();
    connect(compositionList, SIGNAL(finished()),
            this, SLOT(onCompositionSelectorFinished()));

    formulaGenerator = new FormulaGeneratorWorker(this);
    connect(formulaGenerator, SIGNAL(finished()),
            this, SLOT(onFormulaGeneratorFinished()));

    formulaFactory = new FormulaGeneratorBatchWorker(this);
    connect(formulaFactory, SIGNAL(finished(bool)),
            this, SLOT(onFormulaFactoryFinished(bool)));
    connect(formulaFactory, SIGNAL(progressed(double)),
            this, SLOT(onFormulaFactoryProgressed(double)));

    loadPreferences();
    showAllowedElementRanges();
}

MainWindow::~MainWindow()
{
    savePreferences();
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

void MainWindow::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e)
    compositionList->resize(ui->textAllowedElement->width() +
                            ui->buttonAllowedElement->width(),
                            compositionList->height());
}

void MainWindow::loadPreferences()
{
    appConfig.loadFormulaModification(*ui->frameModification);
    appConfig.loadMassModification(*ui->frameMassModification);

    MassTolerance tolerance(appConfig.massTolerance());
    if (tolerance.min != 0.0 || tolerance.max != 0.0)
    {
        ui->textMassToleranceLeft->setText(QString::number(tolerance.min));
        ui->textMassToleranceRight->setText(QString::number(tolerance.max));
    }
    ui->radioMassToleranceRelative->setChecked(tolerance.relative);
    appConfig.loadCompositionSelector(*compositionList);
}

void MainWindow::savePreferences()
{
    appConfig.saveFormulaModification(*ui->frameModification);
    appConfig.saveMassModification(*ui->frameMassModification);

    appConfig.setMassTolerance({ui->textMassToleranceLeft->text().toDouble(),
                                ui->textMassToleranceRight->text().toDouble(),
                                ui->radioMassToleranceRelative->isChecked()});
    appConfig.saveCompositionSelector(*compositionList);
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

void MainWindow::setInputWidgetEnabled(int pageIndex, bool enabled)
{
    if (pageIndex == MC_TAB_INDEX_GETFORMULA) // Get formula
    {
        ui->buttonGetFormula->setEnabled(enabled);
        ui->buttonImportMassFromFile->setEnabled(enabled);
        if (!enabled)
            labelFileLink->hide();
    }
}

void MainWindow::onCompositionSelectorFinished()
{
    compositionList->hide();
    showAllowedElementRanges();
}

void MainWindow::onFormulaModificationChanged()
{
    QString modificationText(
                QString::fromStdString(
                        ui->frameModification->modification().toString()));
    if (!modificationText.isEmpty())
        modificationText.prepend("+[").append("]");

    int charge = ui->frameModification->charge();
    if (charge != 0)
    {
        modificationText.append(modificationText.isEmpty() ? nullptr : ", ")
                        .append(QString::number(abs(charge))
                        .append(charge > 0 ? '+' : '-'));
    }

    ui->textInputFormula->setLabelText(modificationText);
}

void MainWindow::onMassModificationChanged()
{
    QString modificationText(
                QString::fromStdString(
                        ui->frameMassModification->modification().toString()));
    if (!modificationText.isEmpty())
        modificationText.prepend("-[").append("]");

    int charge = ui->frameMassModification->charge();
    if (charge != 0)
    {
        modificationText.append(modificationText.isEmpty() ? nullptr : ", ")
                        .append(QString::number(abs(charge))
                        .append(charge > 0 ? '+' : '-'));
    }

    ui->textInputMass->setLabelText(modificationText);
}

void MainWindow::onFormulaGeneratorFinished()
{
    auto result = formulaGenerator->result();
    if (result.size() < 1)
    {
        ui->statusBar->showMessage("No result found.");
        ui->frameResultFormula->clear();
    }
    else
    {
        ui->statusBar->showMessage(QString("Loading %1 results, "
                                           "please wait...")
                                          .arg(result.size()));
        QCoreApplication::processEvents();
        ui->frameResultFormula->loadResult(result, lastSearchedMass);
        ui->statusBar->showMessage(QString("%1 result(s) found.")
                                          .arg(result.size()));
    }
    setInputWidgetEnabled(MC_TAB_INDEX_GETFORMULA);
    progressBar->hide();
}

void MainWindow::onFormulaFactoryFinished(bool successful)
{
    if (successful)
    {
        QMessageBox::information(this, "Calculation finished",
                                 QString("Finished importing mass list from file"
                                         " and calculating formula. \n"
                                         "%1 mass were processed in total.")
                                        .arg(formulaFactory->countFinished()));
        ui->statusBar->showMessage("Calculation finished.");
        labelFileLink->show();
    }
    else
    {
        QMessageBox::warning(this, "Calculation failed",
                             "Something went wrong during the calculation. \n"
                             "Please check your input file and try again.");
        ui->statusBar->showMessage("Calculation failed.");
    }
    setInputWidgetEnabled(MC_TAB_INDEX_GETFORMULA);
    progressBar->hide();
}

void MainWindow::onFormulaFactoryProgressed(double finishedPercent)
{
    progressBar->setValue(int(finishedPercent));
}

void MainWindow::onLabelFileLinkClicked()
{
    QDesktopServices::openUrl(lastExportFilePath);
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
    f = f + ui->frameModification->modification();

    int charge = ui->frameModification->charge();
    if (charge == 0)
    {
        ui->textResultMass->setText(QString::number(f.toAverageMass()));
        ui->textResultMonoMass->setText(QString::number(f.toMass()));
    }
    else
    {
        ui->textResultMass->setText("(N/A)");
        ui->textResultMonoMass->setText(
                           QString::number((f.toMass() -
                                            AtomMass::electronMass() * charge) /
                                           abs(charge),
                                           'f', 6));
    }
    ui->textResultFormula->setText(
                QString::fromStdString(f.toString(MC_FORMULA_ELEMENT_ORDER)));
}

void MainWindow::on_buttonGetFormula_clicked()
{
    if (formulaGenerator->isRunning())
    {
        QMessageBox::information(this, "Ongoing calculation",
                                 "A calculation task is still being processed.\n"
                                 "Please wait it to be finished "
                                 "before launch a new one.");
            return;
    }

    double mass = ui->textInputMass->text().toDouble();
    if (mass <= 0)
    {
        QMessageBox::warning(this, "Non-positive mass provided",
                             "The mass should be greater than 0");
        return;
    }

    int charge = ui->frameMassModification->charge();
    if (charge != 0)
        mass = mass * abs(charge) + AtomMass::electronMass() * charge;

    Formula modification(ui->frameMassModification->modification());
    double modificationMass = modification.toMass();
    mass -= modificationMass;
    if (mass <= 0)
    {
        QMessageBox::warning(this, "Non-positive mass requested",
                             QString("The input mass minus the mass of "
                                     "modification (%1) should be "
                                     "greater than 0")
                                     .arg(QString::number(modificationMass)));
        return;
    }
    lastSearchedMass = mass;

    double toleranceMin = ui->textMassToleranceLeft->text().toDouble();
    double toleranceMax = ui->textMassToleranceRight->text().toDouble();
    if (ui->radioMassToleranceRelative->isChecked())
    {
        toleranceMin *= mass * 1E-6;
        toleranceMax *= mass * 1E-6;
    }

    auto elementRanges = compositionList->getElementRanges();
    if (elementRanges.isEmpty())
    {
        QMessageBox::warning(this, "No element specified",
                             "Please select at least one element before "
                             "calculating the possible formula.");
        return;
    }

    ui->statusBar->showMessage("Calculating formulae...");
    setInputWidgetEnabled(MC_TAB_INDEX_GETFORMULA, false);
    progressBar->setRange(0, 0);
    progressBar->show();

    formulaGenerator->setElementRanges({elementRanges.cbegin(),
                                        elementRanges.cend()});
    formulaGenerator->setMass(mass + toleranceMin, mass + toleranceMax);
    formulaGenerator->start();
}

void MainWindow::on_buttonAllowedElement_clicked()
{
    if (compositionList->isVisible())
        onCompositionSelectorFinished();
    else
    {
        compositionList->move({ui->textAllowedElement->x(),
                               ui->textAllowedElement->y() +
                               ui->textAllowedElement->height()});
        resizeEvent(nullptr);
        compositionList->show();
    }
}

void MainWindow::on_buttonImportMassFromFile_clicked()
{
    QString filter;
    filter.append(MC_FORMULA_FILE_SUFFIX_ALL).append(";;")
          .append(MC_FORMULA_FILE_SUFFIX_CSV).append(";;");

    if (lastImportFileFilter.isEmpty())
        lastImportFileFilter = MC_FORMULA_FILE_SUFFIX_CSV;
    QString sourceFileName =
            QFileDialog::getOpenFileName(this, "Select a source file",
                                         lastImportFilePath,
                                         filter, &lastImportFileFilter);
    if (sourceFileName.isEmpty())
        return;
    ui->textMassImportFile->setText(sourceFileName);
    lastImportFilePath = sourceFileName;

    if (lastExportFileFilter.isEmpty())
        lastExportFileFilter = lastImportFileFilter;
    QString targetFileName =
            QFileDialog::getSaveFileName(this, "Select a target file",
                                         lastExportFilePath,
                                         filter, &lastExportFileFilter);
    if (targetFileName.isEmpty())
        return;
    lastExportFilePath = targetFileName;

    auto elementRanges = compositionList->getElementRanges();
    if (elementRanges.isEmpty())
    {
        QMessageBox::warning(this, "No element specified",
                             "Please select at least one element before "
                             "calculating the possible formula.");
        return;
    }

    ui->statusBar->showMessage("Calculating formulae...");
    setInputWidgetEnabled(MC_TAB_INDEX_GETFORMULA, false);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->show();

    formulaFactory->setSourceFile(sourceFileName);
    formulaFactory->setTargetFile(targetFileName);
    formulaFactory->setElementOrder(MC_FORMULA_ELEMENT_ORDER);
    formulaFactory->setElementRanges({elementRanges.cbegin(),
                                      elementRanges.cend()});
    formulaFactory->setTolerance(ui->textMassToleranceLeft->text().toDouble(),
                                 ui->textMassToleranceRight->text().toDouble(),
                                 ui->radioMassToleranceRelative->isChecked());
    formulaFactory->start();
}

void MainWindow::on_buttonModification_clicked()
{
    ui->frameModification->setVisible(!ui->frameModification->isVisible());
}

void MainWindow::on_buttonShowImportMassFromFile_clicked()
{
    ui->frameImportMassFromFile->setVisible(
                                    !ui->frameImportMassFromFile->isVisible());
}

void MainWindow::on_buttonShowMassModification_clicked()
{
    ui->frameMassModification->setVisible(
                                    !ui->frameMassModification->isVisible());
}
