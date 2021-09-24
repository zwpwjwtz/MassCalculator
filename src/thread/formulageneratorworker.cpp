#include "formulageneratorworker.h"
#include "../calculator/formulagenerator.h"


FormulaGeneratorWorker::FormulaGeneratorWorker(QObject* parent)
    : QThread(parent)
{
    generator = new FormulaGenerator();
}

FormulaGeneratorWorker::~FormulaGeneratorWorker()
{
    delete generator;
}

void FormulaGeneratorWorker::setElementRanges(
                                        const std::vector<ElementRange>& ranges)
{
    generator->clearElements();
    for (auto i=ranges.cbegin(); i!=ranges.cend(); i++)
        generator->setElement(*i);
}

void FormulaGeneratorWorker::setMass(double minMass, double maxMass)
{
    this->minMass = minMass;
    this->maxMass = maxMass;
}

void FormulaGeneratorWorker::run()
{
    resultList = generator->fromMass(minMass, maxMass);
    emit finished();
}

const std::list<Formula>& FormulaGeneratorWorker::result()
{
    return resultList;
}
