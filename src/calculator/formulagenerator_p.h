#ifndef FORMULAGENERATOR_P_H
#define FORMULAGENERATOR_P_H

#include "formulagenerator.h"


class FormulaGeneratorPrivate
{
public:
    using RangeIterator = std::vector<ElementRange>::const_iterator;
    std::vector<ElementRange> rangeList;

    std::list<Formula>
    calcFromMass(RangeIterator rangeListBegin,
                 RangeIterator rangeListEnd,
                 double minMass, double maxMass);
};

#endif // FORMULAGENERATOR_P_H
