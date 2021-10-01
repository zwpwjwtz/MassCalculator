#include <QSettings>
#include "config.h"
#include "widget/compositionselector.h"
#include "widget/frameformulamodification.h"

#define MC_CONFIG_FORMULA_CHARGE        "formula-charge"
#define MC_CONFIG_FORMULA_MODIFICATION  "formula-modification"
#define MC_CONFIG_MASS_ELEMENTS         "mass-elements"
#define MC_CONFIG_MASS_TOL_MIN          "mass-tolerance-min"
#define MC_CONFIG_MASS_TOL_MAX          "mass-tolerance-max"
#define MC_CONFIG_MASS_TOL_REL          "mass-tolerance-relative"


static QSettings mc_config{"MassCalculator", "Desktop"};

int Config::formulaCharge() const
{
    return(mc_config.value(MC_CONFIG_FORMULA_CHARGE).toInt());
}

void Config::setFormulaCharge(int charge)
{
    mc_config.setValue(MC_CONFIG_FORMULA_CHARGE, charge);
}

void Config::loadFormulaModification(FrameFormulaModification& widget)
{
    Formula f;
    if (f.parse(mc_config.value(MC_CONFIG_FORMULA_MODIFICATION)
                         .toString().toStdString()))
        widget.setModification(f);
}

void Config::saveFormulaModification(const FrameFormulaModification& widget)
{
    mc_config.setValue(MC_CONFIG_FORMULA_MODIFICATION,
                       QString::fromStdString(
                                   widget.modification().toString()));
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
