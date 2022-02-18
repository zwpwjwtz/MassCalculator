#include "isotopicpatternworker.h"
#include "../calculator/isotopicpattern.h"


IsotopicPatternWorker::IsotopicPatternWorker(QObject* parent)
    : QThread(parent)
{
    pattern = new IsotopicPattern;
}

IsotopicPatternWorker::~IsotopicPatternWorker()
{
    delete pattern;
}

void IsotopicPatternWorker::clear()
{
    resultList.clear();
    pattern->setComposition(ChemicalComposition());
}

const ChemicalComposition& IsotopicPatternWorker::composition() const
{
    return pattern->composition();
}

void
IsotopicPatternWorker::setComposition(const ChemicalComposition& composition)
{
    pattern->setComposition(composition);
}

void IsotopicPatternWorker::run()
{
    resultList = pattern->distribution();
    emit finished();
}

void IsotopicPatternWorker::setBinningWidth(double width)
{
    pattern->setBinningWidth(width);
}

void IsotopicPatternWorker::setMaxLength(int maxLength)
{
    pattern->setMaxBinCount(maxLength);
}

const IsotopicPatternWorker::PatternList& IsotopicPatternWorker::result() const
{
    return resultList;
}
