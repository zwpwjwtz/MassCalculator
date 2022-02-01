#include <QMessageBox>
#include "formformulatomass.h"
#include "ui_formformulatomass.h"
#include "global.h"
#include "config.h"
#include "calculator/atommass.h"

#define MC_FORMULA_ELEMENT_ORDER   {"C"}


FormFormulaToMass::FormFormulaToMass(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormFormulaToMass)
{
    ui->setupUi(this);
    ui->frameModification->hide();
    ui->textInputFormula->installEventFilter(this);
    ui->textInputFormula->setLabelTextFormat(Qt::RichText);
    connect(ui->frameModification, SIGNAL(chargeChanged()),
            this, SLOT(onFormulaModificationChanged()));
    connect(ui->frameModification, SIGNAL(modificationChanged()),
            this, SLOT(onFormulaModificationChanged()));
}

FormFormulaToMass::~FormFormulaToMass()
{
    delete ui;
}

void FormFormulaToMass::changeEvent(QEvent *e)
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

void FormFormulaToMass::showEvent(QShowEvent* e)
{
    Q_UNUSED(e)
    ui->textInputFormula->setFocus();
}

void FormFormulaToMass::loadPreferences()
{
    appConfig.loadFormulaModification(*ui->frameModification);
}

void FormFormulaToMass::savePreferences()
{
    appConfig.saveFormulaModification(*ui->frameModification);
}

void FormFormulaToMass::onFormulaModificationChanged()
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

void FormFormulaToMass::on_textInputFormula_returnPressed()
{
    on_buttonGetMass_clicked();
}

void FormFormulaToMass::on_buttonGetMass_clicked()
{
    Formula f;
    bool ok = f.parse(ui->textInputFormula->text().toStdString());
    if (!ok)
    {
        QMessageBox::warning(this, tr("Invalid formula"),
                             tr("The formula that you input is incorrect."));
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
        ui->textResultMass->setText(tr("(N/A)"));
        ui->textResultMonoMass->setText(
                           QString::number((f.toMass() -
                                            AtomMass::electronMass() * charge) /
                                           abs(charge),
                                           'f', 6));
    }
    ui->textResultFormula->setText(
                QString::fromStdString(f.toString(MC_FORMULA_ELEMENT_ORDER)));
}

void FormFormulaToMass::on_buttonModification_clicked()
{
    ui->frameModification->setVisible(!ui->frameModification->isVisible());
}
