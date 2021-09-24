#ifndef FORMULAGENERATORBATCHWORKER_H
#define FORMULAGENERATORBATCHWORKER_H

#include <QThread>
#include "../calculator/elementrange.h"


class FormulaGenerator;

class FormulaGeneratorBatchWorker : public QThread
{
    Q_OBJECT
public:
    explicit FormulaGeneratorBatchWorker(QObject* parent = nullptr);
    ~FormulaGeneratorBatchWorker();

    int countFinished();

    void setElementOrder(const std::vector<std::string> elementNumbers);
    void setElementRanges(const std::vector<ElementRange>& ranges);
    void setTolerance(double min, double max, bool relative = false);

    void setSourceFile(QString filename);
    void setTargetFile(QString filename);

    void run();
    void stop();

signals:
    void finished(bool successful);
    void progressed(double finishedPercent);

protected:
    int finishedCount;
    bool stopRequested;
    bool relativeTolerance;
    double minTolerance, maxTolerance;
    std::vector<std::string> elementOrder;
    QString inputFilename, outputFilename;
    FormulaGenerator* generator;
};

#endif // FORMULAGENERATORBATCHWORKER_H
