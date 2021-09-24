#ifndef FORMULAGENERATORWORKER_H
#define FORMULAGENERATORWORKER_H

#include <QThread>
#include "../calculator/elementrange.h"
#include "../calculator/formula.h"


class FormulaGeneratorWorkerPrivate;

class FormulaGeneratorWorker : public QThread
{
    Q_OBJECT
public:
    explicit FormulaGeneratorWorker(QObject* parent = nullptr);
    ~FormulaGeneratorWorker();

    void setElementRanges(const std::vector<ElementRange>& ranges);
    void setMass(double minMass, double maxMass);

    void run();

    std::list<Formula> result();

signals:
    void finished();

protected:
    FormulaGeneratorWorkerPrivate* d_ptr;
};

#endif // FORMULAGENERATORWORKER_H
