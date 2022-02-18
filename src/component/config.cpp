#include <QSettings>
#include "config.h"
#include "config_field.h"
#include "widget/compositionselector.h"
#include "widget/frameformulamodification.h"


static QSettings mc_config{"MassCalculator", "Desktop"};

void Config::loadFormulaModification(FrameFormulaModification& widget)
{
    Formula f;
    if (f.parse(mc_config.value(MC_CONFIG_FORMULA_MODIFICATION)
                         .toString().toStdString()))
        widget.setModification(f);
    widget.setCharge(mc_config.value(MC_CONFIG_FORMULA_CHARGE).toInt());
}

void Config::saveFormulaModification(const FrameFormulaModification& widget)
{
    mc_config.setValue(MC_CONFIG_FORMULA_MODIFICATION,
                       QString::fromStdString(
                                   widget.modification().toString()));
    mc_config.setValue(MC_CONFIG_FORMULA_CHARGE, widget.charge());
}

void Config::loadMassModification(FrameFormulaModification& widget)
{
    Formula f;
    if (f.parse(mc_config.value(MC_CONFIG_MASS_MODIFICATION)
                         .toString().toStdString()))
        widget.setModification(f);
    widget.setCharge(mc_config.value(MC_CONFIG_MASS_CHARGE).toInt());
}

void Config::saveMassModification(const FrameFormulaModification& widget)
{
    mc_config.setValue(MC_CONFIG_MASS_MODIFICATION,
                       QString::fromStdString(
                                   widget.modification().toString()));
    mc_config.setValue(MC_CONFIG_MASS_CHARGE, widget.charge());
}

MassTolerance Config::massTolerance() const
{
    MassTolerance tolerance{mc_config.value(MC_CONFIG_MASS_TOL_MIN).toDouble(),
                            mc_config.value(MC_CONFIG_MASS_TOL_MAX).toDouble(),
                            mc_config.value(MC_CONFIG_MASS_TOL_REL).toBool()};
    return tolerance;
}

void Config::setMassTolerance(const MassTolerance& tolerance)
{
    mc_config.setValue(MC_CONFIG_MASS_TOL_MIN, tolerance.min);
    mc_config.setValue(MC_CONFIG_MASS_TOL_MAX, tolerance.max);
    mc_config.setValue(MC_CONFIG_MASS_TOL_REL, tolerance.relative);
}

void Config::loadCompositionSelector(CompositionSelector& widget)
{
    QStringList allowElements = mc_config.value(MC_CONFIG_MASS_ELEMENTS)
                                         .toStringList();
    if (allowElements.count() == 0)
        return;
    widget.clear();

    QStringList tempStringList;
    for (int i=0; i<allowElements.count(); i++)
    {
        tempStringList = allowElements[i].split(',');
        if (tempStringList.count() == 3)
            widget.addElement(tempStringList[0].toInt(),
                              tempStringList[1].toInt(),
                              tempStringList[2].toInt());
    }
}

void Config::saveCompositionSelector(const CompositionSelector& widget)
{
    QStringList rangeList;
    auto composition = widget.getElementRanges();
    for (auto i=composition.cbegin(); i!=composition.cend(); i++)
    {
        rangeList.push_back(QString("%1,%2,%3")
                                   .arg(QString::number(i->atomNumber))
                                   .arg(QString::number(i->minCount))
                                   .arg(QString::number(i->maxCount)));
    }
    mc_config.setValue(MC_CONFIG_MASS_ELEMENTS, rangeList);
}


int Config::numberPrecision(PrecisionType type) const
{
    QVariant value = mc_config.value(QString("%1-%2")
                                            .arg(MC_CONFIG_MASS_PRECISION)
                                            .arg(QString::number(int(type))));
    if (value.isNull())
        return 4;
    else
        return value.toInt();
}

void Config::setNumberPrecision(PrecisionType type, int digits)
{
    mc_config.setValue(QString("%1-%2")
                              .arg(MC_CONFIG_MASS_PRECISION)
                              .arg(int(type)),
                       digits);
}

double Config::binningWidth() const
{
    QVariant value = mc_config.value(MC_CONFIG_ISOTOPIC_BIN_WIDTH).toDouble();
    if (value.isNull())
        return 0.5;
    else
        return value.toDouble();
}

void Config::setBinningWidth(double width)
{
    mc_config.setValue(MC_CONFIG_ISOTOPIC_BIN_WIDTH, width);
}

int Config::maxIsotopicCount() const
{
    QVariant value = mc_config.value(MC_CONFIG_ISOTOPIC_BIN_COUNT).toInt();
    if (value.isNull())
        return 10;
    else
        return value.toInt();
}

void Config::setMaxIsotopicCount(int count)
{
    mc_config.setValue(MC_CONFIG_ISOTOPIC_BIN_COUNT, count);
}

QString Config::localeName() const
{
    QVariant value = mc_config.value(MC_CONFIG_LOCALE_NAME).toString();
    if (value.isNull())
        return defaultLocaleName();
    else
        return value.toString();
}

QString Config::defaultLocaleName()
{
    return QLocale::system().name();
}

void Config::setLocaleName(const QString& locale)
{
    mc_config.setValue(MC_CONFIG_LOCALE_NAME, locale);
}
