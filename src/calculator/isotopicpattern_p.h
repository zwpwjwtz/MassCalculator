#ifndef ISOTOPICPATTERN_P_H
#define ISOTOPICPATTERN_P_H

#include <list>
#include "chemicalcomposition.h"


class IsotopicPatternPrivate
{
public:
    double interval;
    int maxIntervalCount;
    ChemicalComposition composition;

    static std::list<double> enumerateIsotopeMass(
            std::vector<std::pair<double, double>>::const_iterator isotope,
            std::vector<std::pair<double, double>>::const_iterator isotopeEnd,
            int atomCount);

    static void binMassList(std::list<double>& mass, double interval);
};

#endif // ISOTOPICPATTERN_P_H
