#ifndef FORMULAGENERATOR_H
#define FORMULAGENERATOR_H

#include <list>
#include <vector>
#include "formula.h"
#include "elementrange.h"


class FormulaGeneratorPrivate;

class FormulaGenerator
{
public:
    FormulaGenerator();
    ~FormulaGenerator();

    std::list<int> allElements() const;
    void clearElements();

    ElementRange getElement(int atomNumber) const;
    void setElement(const ElementRange range);
    void removeElement(int atomNumber);

    std::list<Formula> fromMass(double minMass, double maxMass) const;

protected:
    FormulaGeneratorPrivate* d_ptr;
};

#endif // FORMULAGENERATOR_H
