#ifndef CONFIG_H
#define CONFIG_H

#include <QString>


class CompositionSelector;
class CompositionTemplateList;
class FrameFormulaModification;

struct MassTolerance
{
    double min, max;
    bool relative;
};

class Config
{
public:
    enum PrecisionType
    {
        AveragedMass,
        IsotopicMass,
        IsotopicPatternMass
    };

    void loadFormulaModification(FrameFormulaModification& widget) const;
    void saveFormulaModification(const FrameFormulaModification& widget);

    void loadMassModification(FrameFormulaModification& widget) const;
    void saveMassModification(const FrameFormulaModification& widget);

    MassTolerance massTolerance() const;
    void setMassTolerance(const MassTolerance& tolerance);

    void loadCompositionSelector(CompositionSelector& widget) const;
    void saveCompositionSelector(const CompositionSelector& widget);

    void loadCompositionTemplates(CompositionTemplateList& widget) const;
    void saveCompositionTemplates(const CompositionTemplateList& widget);

    int numberPrecision(PrecisionType type) const;
    void setNumberPrecision(PrecisionType type, int digits);

    double binningWidth() const;
    void setBinningWidth(double width);

    int maxIsotopicCount() const;
    void setMaxIsotopicCount(int count);

    QString localeName() const;
    static QString defaultLocaleName();
    void setLocaleName(const QString& locale);
};

#endif // CONFIG_H
