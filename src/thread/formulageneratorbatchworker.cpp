#include <QFile>
#include "formulageneratorbatchworker.h"
#include "../calculator/formulagenerator.h"

#define MC_FORMULA_BATCH_INPUT_COLUMN_SEP    ","
#define MC_FORMULA_BATCH_OUTPUT_LINE_SEP     "\n"
#define MC_FORMULA_BATCH_OUTPUT_COLUMN_SEP   ","


FormulaGeneratorBatchWorker::FormulaGeneratorBatchWorker(QObject* parent)
    : QThread (parent)
{
    generator = new FormulaGenerator();
    relativeTolerance = false;
    finishedCount = 0;
}

FormulaGeneratorBatchWorker::~FormulaGeneratorBatchWorker()
{
    delete generator;
}

int FormulaGeneratorBatchWorker::countFinished()
{
    return finishedCount;
}

void FormulaGeneratorBatchWorker::setSourceFile(QString filename)
{
    this->inputFilename = filename;
}

void FormulaGeneratorBatchWorker::setTargetFile(QString filename)
{
    this->outputFilename = filename;
}

void FormulaGeneratorBatchWorker::setElementOrder(
                                const std::vector<std::string> elementNumbers)
{
    elementOrder = elementNumbers;
}

void FormulaGeneratorBatchWorker::setElementRanges(
                                        const std::vector<ElementRange>& ranges)
{
    generator->clearElements();
    for (auto i=ranges.cbegin(); i!=ranges.cend(); i++)
        generator->setElement(*i);
}

void FormulaGeneratorBatchWorker::setTolerance(double min, double max,
                                               bool relative)
{
    minTolerance = min;
    maxTolerance = max;
    relativeTolerance = relative;
}

void FormulaGeneratorBatchWorker::run()
{
    QFile input(this->inputFilename);
    if (!input.open(QFile::ReadOnly | QFile::Text))
    {
        emit finished(false);
        return;
    }
    QFile output(this->outputFilename);
    if (!output.open(QFile::WriteOnly | QFile::Text))
    {
        emit finished(false);
        return;
    }

    // Parse the input file (assuming CSV format)
    int p;
    bool conversionOK;
    double mass, minMass, maxMass;
    QByteArray buffer;
    std::list<Formula> resultList;
    std::list<Formula>::const_iterator i;
    stopRequested = false;
    finishedCount = 0;
    while (!input.atEnd() && !stopRequested)
    {
        // Parse the input line-by-line
        buffer = input.readLine();
        p = buffer.indexOf(MC_FORMULA_BATCH_INPUT_COLUMN_SEP);
        if (p > 0)
        {
            // Line containing multiple fields: take the first one
            mass = buffer.mid(0, p - 1).replace('"', nullptr)
                                       .toDouble(&conversionOK);
        }
        else if (p < 0)
        {
            // Line contaning only one field
            mass = buffer.replace('"', nullptr).toDouble(&conversionOK);
        }
        else
        {
            // The first field is empty: skip it
            continue;
        }
        if (!conversionOK)
            continue;

        // Calculate
        if (relativeTolerance)
        {
            minMass = mass + mass * minTolerance * 1E-6;
            maxMass = mass + mass * maxTolerance * 1E-6;
        }
        else
        {
            minMass = mass + minTolerance;
            maxMass = mass + maxTolerance;
        }
        resultList = generator->fromMass(minMass, maxMass);

        // Output
        output.write(QByteArray::number(mass));
        if (resultList.size() > 0)
        {
            for (auto i=resultList.cbegin(); i!=resultList.cend(); i++)
            {
                output.write(MC_FORMULA_BATCH_OUTPUT_COLUMN_SEP);
                output.write(i->toString(elementOrder).c_str());
            }
        }
        output.write(MC_FORMULA_BATCH_OUTPUT_LINE_SEP);

        finishedCount++;
        emit progressed(100.0 * input.pos() / input.size());
    }

    emit finished(true);
}

void FormulaGeneratorBatchWorker::stop()
{
    stopRequested = true;
}
