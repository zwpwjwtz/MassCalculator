#ifndef ISOTOPICPATTERN_H
#define ISOTOPICPATTERN_H

#include "chemicalcomposition.h"


class IsotopicPatternPrivate;

class IsotopicPattern
{
public:
    const double defaultBinningWidth = 1.0;
    const int defaultMaxBinCount = 100;

    IsotopicPattern();
    ~IsotopicPattern();

    ChemicalComposition composition() const;
    bool setComposition(const ChemicalComposition& composition);

    double binningWidth() const;
    void setBinningWidth(double width);
    int maxBinCount() const;
    void setMaxBinCount(int count);

    // The overall (convoluted) isotopic pattern
    std::vector<std::pair<double, double>> distribution() const;

    // Isotopic pattern for a specific element
    std::vector<std::pair<double, double>> distribution(int atomNumber) const;

protected:
    IsotopicPatternPrivate* d_ptr;
};

#endif // ISOTOPICPATTERN_H
