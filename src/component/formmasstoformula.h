#ifndef FORMMASSTOFORMULA_H
#define FORMMASSTOFORMULA_H

#include <QWidget>


class CompositionSelector;
class FormulaGeneratorWorker;
class FormulaGeneratorBatchWorker;
class QLabel;
class QProgressBar;

namespace Ui {
class FormMassToFormula;
}

class FormMassToFormula : public QWidget
{
    Q_OBJECT

public:
    explicit FormMassToFormula(QWidget *parent = nullptr);
    ~FormMassToFormula();

    void loadPreferences();
    void savePreferences();

protected:
    void changeEvent(QEvent* e);
    void resizeEvent(QResizeEvent* e);
    void showEvent(QShowEvent* e);

private:
    Ui::FormMassToFormula *ui;
    FormulaGeneratorWorker* formulaGenerator;
    FormulaGeneratorBatchWorker* formulaFactory;
    CompositionSelector* compositionList;

    double lastSearchedMass;
    QString lastImportFilePath, lastExportFilePath;
    QString lastImportFileFilter, lastExportFileFilter;

    void showAllowedElementRanges();
    void setInputWidgetEnabled(bool enabled = true);

private slots:
    void onCompositionSelectorFinished();
    void onMassModificationChanged();
    void onFormulaGeneratorFinished();
    void onFormulaFactoryFinished(bool successful);
    void onFormulaFactoryProgressed(double finishedPercent);
    void onLabelFileLinkClicked();

    void on_textInputMass_returnPressed();
    void on_buttonGetFormula_clicked();
    void on_buttonAllowedElement_clicked();
    void on_buttonImportMassFromFile_clicked();
    void on_buttonShowImportMassFromFile_clicked();
    void on_buttonShowMassModification_clicked();
};

#endif // FORMMASSTOFORMULA_H
