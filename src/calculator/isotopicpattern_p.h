#ifndef ISOTOPICPATTERN_P_H
#define ISOTOPICPATTERN_P_H

#include <list>
#include "chemicalcomposition.h"


class IsotopicPatternPrivate
{
public:
    using IsotopeAbundanceList = std::vector<std::pair<double, double>>;
    using IsotopeAbundanceIterator = IsotopeAbundanceList::const_iterator;
    using ElementList = std::list<std::pair<int, IsotopeAbundanceList>>;
    using ElementListIterator = ElementList::const_iterator;

    double interval;
    int maxIntervalCount;
    ChemicalComposition composition;

    static unsigned long long combinatorial(unsigned int N, unsigned int r);
    static void binMassList(std::list<double>& mass, double interval);
    static std::list<double>
    enumerateMass(IsotopeAbundanceIterator isotopeBegin,
                  IsotopeAbundanceIterator isotopeEnd,
                  int atomCount);
    static double
    jointProbability(double mass,
                     double tolerance,
                     double massShift,
                     const std::list<double>& massList,
                     const ElementListIterator& elementListBegin,
                     const ElementListIterator& elementListEnd);
    static double
    marginalProbability(double residualMass, double tolerance,
                        int atomCount,
                        IsotopeAbundanceIterator isotopeBegin,
                        const IsotopeAbundanceIterator& isotopeEnd);
};

#endif // ISOTOPICPATTERN_P_H
