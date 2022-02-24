#include "dialogformulalistfilter.h"
#include "ui_dialogformulalistfilter.h"


DialogFormulaListFilter::DialogFormulaListFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogFormulaListFilter)
{
    ui->setupUi(this);
    ui->frameUnsaturationOption->setEnabled(ui->checkUnsaturation->isChecked());
}

DialogFormulaListFilter::~DialogFormulaListFilter()
{
    delete ui;
}

void DialogFormulaListFilter::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

bool DialogFormulaListFilter::isActivated() const
{
    return filterSaturationDegree();
}

bool DialogFormulaListFilter::filterSaturationDegree() const
{
    return ui->checkUnsaturation->isChecked();
}

void DialogFormulaListFilter::setSaturationDegreeRange(double min, double max)
{
    ui->spinUnsaturationFrom->setValue(min);
    ui->spinUnsaturationTo->setValue(max);
}

double DialogFormulaListFilter::saturationDegreeMin() const
{
    return ui->spinUnsaturationFrom->value();
}

double DialogFormulaListFilter::saturationDegreeMax() const
{
    return ui->spinUnsaturationTo->value();
}

void DialogFormulaListFilter::on_checkUnsaturation_stateChanged(int arg1)
{
    ui->frameUnsaturationOption->setEnabled(arg1 == Qt::CheckState::Checked);
}
