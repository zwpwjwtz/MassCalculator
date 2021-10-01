#ifndef CONFIG_H
#define CONFIG_H

class CompositionSelector;
class FrameFormulaModification;

struct MassTolerance
{
    double min, max;
    bool relative;
};

class Config
{
public:
    int formulaCharge() const;
    void setFormulaCharge(int charge);

    void loadFormulaModification(FrameFormulaModification& widget);
    void saveFormulaModification(const FrameFormulaModification& widget);

    MassTolerance massTolerance() const;
    void setMassTolerance(const MassTolerance& tolerance);

    void loadCompositionSelector(CompositionSelector& widget);
    void saveCompositionSelector(const CompositionSelector& widget);
};

#endif // CONFIG_H
