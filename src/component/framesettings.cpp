#include <QTranslator>
#include "framesettings.h"
#include "ui_framesettings.h"
#include "global.h"
#include "component/config.h"

#define MC_SETTINGS_FIELD_PRECISION_ISOTOPIC  "isotopic"
#define MC_SETTINGS_FIELD_PRECISION_AVERAGED  "averaged"


FrameSettings::FrameSettings(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameSettings)
{
    ui->setupUi(this);
    ui->comboLanguage->setItemData(0, "zh_CN");
    ui->comboLanguage->setItemData(1, "en_US");

    loadSettings();
}

FrameSettings::~FrameSettings()
{
    delete ui;
}

void FrameSettings::changeEvent(QEvent *e)
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

void FrameSettings::loadSettings()
{
    ui->spinMassPrecisionAveraged->setValue(
                        appConfig.numberPrecision(Config::AveragedMass));
    ui->spinMassPrecisionMonoisotopic->setValue(
                        appConfig.numberPrecision(Config::IsotopicMass));
    ui->spinMassPrecisionIsotopicPattern->setValue(
                        appConfig.numberPrecision(Config::IsotopicPatternMass));
    ui->spinBinningWidth->setValue(appConfig.binningWidth());
    ui->spinMaxIsotopicCount->setValue(appConfig.maxIsotopicCount());

    // Load the language table
    int i;
    QString localeName(appConfig.localeName());
    for (i=0; i<ui->comboLanguage->count(); i++)
    {
        if (ui->comboLanguage->itemData(i).toString() == localeName)
            break;
    }
    if (i >= ui->comboLanguage->count())
        i = 0;
    ui->comboLanguage->setCurrentIndex(i);

    // Manually trigger the locale change is needed during the initialization
    on_comboLanguage_currentIndexChanged(i);
}

void FrameSettings::on_comboLanguage_currentIndexChanged(int index)
{
    appConfig.setLocaleName(ui->comboLanguage->currentData().toString());
    appTranslator.load(QString(":/translation/MassCalculator_")
                       .append(ui->comboLanguage->itemData(index).toString()));
}

void FrameSettings::on_spinMassPrecisionAveraged_valueChanged(int arg1)
{
    appConfig.setNumberPrecision(Config::AveragedMass, arg1);
}

void FrameSettings::on_spinMassPrecisionMonoisotopic_valueChanged(int arg1)
{
    appConfig.setNumberPrecision(Config::IsotopicMass, arg1);
}

void FrameSettings::on_spinMassPrecisionIsotopicPattern_valueChanged(int arg1)
{
    appConfig.setNumberPrecision(Config::IsotopicPatternMass, arg1);
}

void FrameSettings::on_spinBinningWidth_valueChanged(double arg1)
{
    appConfig.setBinningWidth(arg1);
}

void FrameSettings::on_spinMaxIsotopicCount_valueChanged(int arg1)
{
    appConfig.setMaxIsotopicCount(arg1);
}
