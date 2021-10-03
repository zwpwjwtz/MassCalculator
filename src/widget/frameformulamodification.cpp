#include "frameformulamodification.h"
#include "ui_frameformulamodification.h"
#include "../calculator/formula.h"
#include "../calculator/formula_adduct.h"


FrameFormulaModification::FrameFormulaModification(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameFormulaModification)
{
    ui->setupUi(this);
    loadDefaultModification();

    connect(ui->spinCharge, SIGNAL(valueChanged(int)),
            this, SIGNAL(chargeChanged()));
    connect(ui->comboAdduct, SIGNAL(currentIndexChanged(int)),
            this, SIGNAL(modificationChanged()));
}

FrameFormulaModification::~FrameFormulaModification()
{
    delete ui;
}

void FrameFormulaModification::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

int FrameFormulaModification::charge() const
{
    return ui->spinCharge->value();
}

void FrameFormulaModification::setCharge(int charge)
{
    ui->spinCharge->setValue(charge);
}

Formula FrameFormulaModification::modification() const
{
    int index = ui->comboAdduct->currentIndex();
    if (index >= 0 && index < formulaList.count())
        return formulaList[index];
    else
        return Formula();
}

void FrameFormulaModification::setModification(const Formula& formula,
                                               const QString& text)
{
    int i;
    for (i=0; i<formulaList.count(); i++)
    {
        if (formulaList[i] == formula)
            break;
    }
    if (i == formulaList.count())
    {
        // No matched modification; add a new entry for it
        formulaList.push_back(formula);
        ui->comboAdduct->addItem(text);
    }
    else
        ui->comboAdduct->setCurrentIndex(i);
}

void FrameFormulaModification::loadDefaultModification()
{
    int i, j;
    Formula formula;

    // Add a "None" entry for no modification
    formulaList.push_back(formula);
    ui->comboAdduct->addItem("(None)");

    // Append the default modification list
    for (i=0; i<MC_FORMULA_ADDUCT_MAX; i++)
    {
        formula.clear();
        for (j=0; j<MC_FORMULA_ADDUCT_ELEMENT_MAX; j++)
        {
            if (mc_formula_adduct_composition[i][j][0] == 0)
                break;
            formula.setIsotope(mc_formula_adduct_composition[i][j][0],
                               mc_formula_adduct_composition[i][j][1],
                               mc_formula_adduct_composition[i][j][2]);
        }
        if (!formula.isEmpty())
        {
            formulaList.push_back(formula);
            ui->comboAdduct->addItem(mc_formula_adduct_name[i]);
        }
    }
}
