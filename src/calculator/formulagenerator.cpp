#include "formulagenerator_p.h"
#include "atommass.h"


FormulaGenerator::FormulaGenerator()
{
    d_ptr = new FormulaGeneratorPrivate();
}

FormulaGenerator::~FormulaGenerator()
{
    delete d_ptr;
}

std::list<int> FormulaGenerator::allElements() const
{
    std::list<int> elements;
    for (auto i=d_ptr->rangeList.cbegin(); i!=d_ptr->rangeList.cend(); i++)
        elements.push_back(i->atomNumber);
    return elements;
}

void FormulaGenerator::clearElements()
{
    d_ptr->rangeList.clear();
}

ElementRange FormulaGenerator::getElement(int atomNumber) const
{
    for (auto i=d_ptr->rangeList.cbegin(); i!=d_ptr->rangeList.cend(); i++)
    {
        if (i->atomNumber == atomNumber)
            return *i;
    }
    return ElementRange();
}

void FormulaGenerator::setElement(const ElementRange range)
{
    for (auto i=d_ptr->rangeList.begin(); i!=d_ptr->rangeList.end(); i++)
    {
        if (i->atomNumber == range.atomNumber)
        {
            *i = range;
            return;
        }
    }
    d_ptr->rangeList.push_back(range);
}

void FormulaGenerator::removeElement(int atomNumber)
{
    for(auto i = d_ptr->rangeList.begin(); i!=d_ptr->rangeList.end(); i++)
    {
        if (i->atomNumber == atomNumber)
        {
            d_ptr->rangeList.erase(i);
            break;
        }
    }
}

std::list<Formula>
FormulaGenerator::fromMass(double minMass, double maxMass) const
{
    if (d_ptr->rangeList.size() < 1 || minMass <= 0 || minMass > maxMass)
        return std::list<Formula>();

    // Calculate possible combination of elements recursively
    // Each element (with its range) is dealed in a layer
    return d_ptr->calcFromMass(d_ptr->rangeList.cbegin(),
                               d_ptr->rangeList.cend(),
                               minMass, maxMass);
}

std::list<Formula>
FormulaGeneratorPrivate::calcFromMass(RangeIterator rangeListBegin,
                                      RangeIterator rangeListEnd,
                                      double minMass, double maxMass)
{
    int atomNumber = rangeListBegin->atomNumber;
    double elementMass = AtomMass::monoisotopicMass(atomNumber);

    int minCount = rangeListBegin->minCount;
    int maxCount = rangeListBegin->maxCount;
    if (minCount > maxCount)
        std::swap(minCount, maxCount);

    int i;
    std::list<Formula> result;
    if (rangeListBegin + 1 == rangeListEnd)
    {
        Formula newFormula;
        for (i=minCount; i<=maxCount; i++)
        {
            if (minMass <= i * elementMass && maxMass >= i * elementMass)
            {
                newFormula.setElement(atomNumber, i);
                result.push_back(newFormula);
            }
        }
    }
    else
    {
        std::list<Formula> tempResult;
        std::list<Formula>::iterator j;
        for (i=minCount; i<=maxCount; i++)
        {
            tempResult = calcFromMass(rangeListBegin+1, rangeListEnd,
                                      minMass - i * elementMass,
                                      maxMass - i * elementMass);
            if (tempResult.size() > 0)
            {
                for (j = tempResult.begin(); j!=tempResult.end(); j++)
                    j->setElement(atomNumber, i);
                result.splice(result.end(), tempResult);
            }
        }
    }
    return result;
}
