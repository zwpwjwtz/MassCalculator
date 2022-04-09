#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QUrl>
#include "formmasstoformula.h"
#include "ui_formmasstoformula.h"
#include "global.h"
#include "config.h"
#include "calculator/atommass.h"
#include "thread/formulageneratorworker.h"
#include "thread/formulageneratorbatchworker.h"
#include "widget/compositionselector.h"
#include "widget/frameformulalist.h"

#define MC_FORMULA_ELEMENT_ORDER   {"C"}

#define MC_FORMULA_FILE_SUFFIX_ALL "All (*)"
#define MC_FORMULA_FILE_SUFFIX_CSV "CSV (*.csv)"


FormMassToFormula::FormMassToFormula(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMassToFormula)
{
    ui->setupUi(this);
    ui->frameMassModification->hide();
    ui->frameImportMassFromFile->hide();
    ui->textInputMass->installEventFilter(this);
    ui->textInputMass->setLabelTextFormat(Qt::RichText);
    ui->textMassToleranceLeft->installEventFilter(this);
    ui->textMassToleranceRight->installEventFilter(this);
    connect(ui->frameMassModification, SIGNAL(chargeChanged()),
            this, SLOT(onMassModificationChanged()));
    connect(ui->frameMassModification, SIGNAL(modificationChanged()),
            this, SLOT(onMassModificationChanged()));

    ui->labelFileLink->setVisible(false);
    ui->labelFileLink->setTextFormat(Qt::TextFormat::RichText);
    connect(ui->labelFileLink, SIGNAL(linkActivated(const QString&)),
            this, SLOT(onLabelFileLinkClicked()));

    ui->progressBar->setValue(0);
    ui->progressBar->setVisible(false);

    compositionList = new CompositionSelector(this);
    compositionList->setAutoFillBackground(true);
    compositionList->setWindowFlags(Qt::WindowType::Popup);
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
}

FormMassToFormula::~FormMassToFormula()
{
    delete ui;
}

void FormMassToFormula::changeEvent(QEvent* e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void FormMassToFormula::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e)
    compositionList->resize(ui->comboAllowedElement->width() +
                            ui->buttonAllowedElement->width(),
                            compositionList->height());
}

void FormMassToFormula::showEvent(QShowEvent* e)
{
    Q_UNUSED(e)
    ui->textInputMass->setFocus();
}

void FormMassToFormula::loadPreferences()
{
    appConfig.loadMassModification(*ui->frameMassModification);

    MassTolerance tolerance(appConfig.massTolerance());
    if (tolerance.min != 0.0 || tolerance.max != 0.0)
    {
        ui->textMassToleranceLeft->setText(QString::number(tolerance.min));
        ui->textMassToleranceRight->setText(QString::number(tolerance.max));
    }
    ui->radioMassToleranceRelative->setChecked(tolerance.relative);
    appConfig.loadCompositionTemplates(*(ui->comboAllowedElement));
    appConfig.loadCompositionSelector(*compositionList);
    ui->comboAllowedElement->setCurrentComposition(
                                        compositionList->getElementRanges(),
                                        compositionList->getRemark());
}

void FormMassToFormula::savePreferences()
{
    appConfig.saveMassModification(*ui->frameMassModification);

    appConfig.setMassTolerance({ui->textMassToleranceLeft->text().toDouble(),
                                ui->textMassToleranceRight->text().toDouble(),
                                ui->radioMassToleranceRelative->isChecked()});
    appConfig.saveCompositionTemplates(*(ui->comboAllowedElement));
    appConfig.saveCompositionSelector(*compositionList);
}

void FormMassToFormula::setInputWidgetEnabled(bool enabled)
{
    ui->buttonGetFormula->setEnabled(enabled);
    ui->buttonImportMassFromFile->setEnabled(enabled);
    if (!enabled)
        ui->labelFileLink->hide();
}

void FormMassToFormula::onCompositionSelectorFinished()
{
    compositionList->hide();
    ui->comboAllowedElement->setCurrentComposition(
                                    compositionList->getElementRanges(),
                                    compositionList->getRemark(),
                                    true);
}

void FormMassToFormula::onMassModificationChanged()
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

void FormMassToFormula::onFormulaGeneratorFinished()
{
    auto result = formulaGenerator->result();
    if (result.size() < 1)
    {
        ui->labelStatus->setText(tr("No result found."));
        ui->frameResultFormula->clear();
    }
    else
    {
        ui->labelStatus->setText(QString(tr("Loading %1 results, "
                                              "please wait..."))
                                          .arg(result.size()));
        QCoreApplication::processEvents();
        ui->frameResultFormula->loadResult(result, lastSearchedMass);
        ui->labelStatus->setText(QString(tr("%1 result(s) found."))
                                          .arg(result.size()));
    }
    setInputWidgetEnabled();
    ui->progressBar->hide();
}

void FormMassToFormula::onFormulaFactoryFinished(bool successful)
{
    if (successful)
    {
        QMessageBox::information(this, tr("Calculation finished"),
                                 QString(tr("Finished importing mass list from "
                                            "file and calculating formula. \n"
                                            "%1 mass were processed in total."))
                                        .arg(formulaFactory->countFinished()));
        ui->labelStatus->setText(tr("Calculation finished."));
        ui->labelFileLink->show();
    }
    else
    {
        QMessageBox::warning(this, tr("Calculation failed"),
                             tr("Something went wrong during the calculation.\n"
                                "Please check your input file and try again."));
        ui->labelStatus->setText(tr("Calculation failed."));
    }
    setInputWidgetEnabled();
    ui->progressBar->hide();
}

void FormMassToFormula::onFormulaFactoryProgressed(double finishedPercent)
{
    ui->progressBar->setValue(int(finishedPercent));
}

void FormMassToFormula::onLabelFileLinkClicked()
{
    QDesktopServices::openUrl(lastExportFilePath);
}

void FormMassToFormula::on_textInputMass_returnPressed()
{
    on_buttonGetFormula_clicked();
}

void FormMassToFormula::on_buttonGetFormula_clicked()
{
    if (formulaGenerator->isRunning())
    {
        QMessageBox::information(this, tr("Ongoing calculation"),
                                 tr("A calculation task is still being "
                                    "processed.\n"
                                    "Please wait it to be finished "
                                    "before launch a new one."));
            return;
    }

    double mass = ui->textInputMass->text().toDouble();
    if (mass <= 0)
    {
        QMessageBox::warning(this, tr("Non-positive mass provided"),
                             tr("The mass should be greater than 0"));
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
        QMessageBox::warning(this, tr("Non-positive mass requested"),
                             QString(tr("The input mass minus the mass of "
                                        "modification (%1) should be "
                                        "greater than 0"))
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
        QMessageBox::warning(this, tr("No element specified"),
                             tr("Please select at least one element before "
                                "calculating the possible formula."));
        return;
    }

    ui->labelStatus->setText(tr("Calculating formulae..."));
    setInputWidgetEnabled(false);
    ui->progressBar->setRange(0, 0);
    ui->progressBar->show();

    formulaGenerator->setElementRanges({elementRanges.cbegin(),
                                        elementRanges.cend()});
    formulaGenerator->setMass(mass + toleranceMin, mass + toleranceMax);
    formulaGenerator->start();
}

void FormMassToFormula::on_buttonAllowedElement_clicked()
{
    if (compositionList->isVisible())
        onCompositionSelectorFinished();
    else
    {
        compositionList->move(mapToGlobal(
                              {ui->comboAllowedElement->x(),
                               ui->comboAllowedElement->y() +
                               ui->comboAllowedElement->height()}));
        resizeEvent(nullptr);
        compositionList->show();
    }
}

void FormMassToFormula::on_buttonImportMassFromFile_clicked()
{
    QString filter;
    filter.append(MC_FORMULA_FILE_SUFFIX_ALL).append(";;")
          .append(MC_FORMULA_FILE_SUFFIX_CSV).append(";;");

    if (lastImportFileFilter.isEmpty())
        lastImportFileFilter = MC_FORMULA_FILE_SUFFIX_CSV;
    QString sourceFileName =
            QFileDialog::getOpenFileName(this, tr("Select a source file"),
                                         lastImportFilePath,
                                         filter, &lastImportFileFilter);
    if (sourceFileName.isEmpty())
        return;
    ui->textMassImportFile->setText(sourceFileName);
    lastImportFilePath = sourceFileName;

    if (lastExportFileFilter.isEmpty())
        lastExportFileFilter = lastImportFileFilter;
    QString targetFileName =
            QFileDialog::getSaveFileName(this, tr("Select a target file"),
                                         lastExportFilePath,
                                         filter, &lastExportFileFilter);
    if (targetFileName.isEmpty())
        return;
    lastExportFilePath = targetFileName;

    auto elementRanges = compositionList->getElementRanges();
    if (elementRanges.isEmpty())
    {
        QMessageBox::warning(this, tr("No element specified"),
                             tr("Please select at least one element before "
                                "calculating the possible formula."));
        return;
    }

    ui->labelStatus->setText(tr("Calculating formulae..."));
    setInputWidgetEnabled(false);
    ui->progressBar->setRange(0, 100);
    ui->progressBar->setValue(0);
    ui->progressBar->show();

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

void FormMassToFormula::on_buttonShowImportMassFromFile_clicked()
{
    ui->frameImportMassFromFile->setVisible(
                                    !ui->frameImportMassFromFile->isVisible());
}

void FormMassToFormula::on_buttonShowMassModification_clicked()
{
    ui->frameMassModification->setVisible(
                                    !ui->frameMassModification->isVisible());
}

void FormMassToFormula::on_comboAllowedElement_activated(int index)
{
    if (index < 0)
        return;

    compositionList->setElementRanges(
                            ui->comboAllowedElement->composition(index));
    compositionList->setRemark(ui->comboAllowedElement->remark(index));
}
