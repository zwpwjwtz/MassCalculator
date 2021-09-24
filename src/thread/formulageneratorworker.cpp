#include "formulageneratorworker.h"
#include "formulageneratorworker_p.h"


FormulaGeneratorWorker::FormulaGeneratorWorker(QObject* parent)
    : QThread(parent)
{
    d_ptr = new FormulaGeneratorWorkerPrivate();
}

FormulaGeneratorWorker::~FormulaGeneratorWorker()
{
    delete d_ptr;
}

void FormulaGeneratorWorker::setElementRanges(
                                        const std::vector<ElementRange>& ranges)
{
    d_ptr->generator.clearElements();
    for (auto i=ranges.cbegin(); i!=ranges.cend(); i++)
        d_ptr->generator.setElement(*i);
}

void FormulaGeneratorWorker::setMass(double minMass, double maxMass)
{
    d_ptr->minMass = minMass;
    d_ptr->maxMass = maxMass;
}

void FormulaGeneratorWorker::run()
{
    d_ptr->result = d_ptr->generator.fromMass(d_ptr->minMass, d_ptr->maxMass);
    emit finished();
}

std::list<Formula> FormulaGeneratorWorker::result()
{
    return d_ptr->result;
}
