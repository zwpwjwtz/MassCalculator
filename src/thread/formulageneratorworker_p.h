#ifndef FORMULAGENERATORWORKER_P_H
#define FORMULAGENERATORWORKER_P_H


#include "../calculator/formulagenerator.h"


class FormulaGeneratorWorkerPrivate
{
public:
    FormulaGenerator generator;
    double minMass, maxMass;
    std::list<Formula> result;
};

#endif // FORMULAGENERATORWORKER_P_H
