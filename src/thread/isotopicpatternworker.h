#ifndef ISOTOPICPATTERNWORKER_H
#define ISOTOPICPATTERNWORKER_H

#include <QThread>
#include "calculator/chemicalcomposition.h"


class IsotopicPattern;

class IsotopicPatternWorker : public QThread
{
    Q_OBJECT
public:
    using PatternList = std::vector<std::pair<double, double>>;
    IsotopicPatternWorker(QObject* parent = nullptr);
    ~IsotopicPatternWorker();

    void clear();
    const ChemicalComposition& composition() const;
    void setComposition(const ChemicalComposition& composition);

    void run();

    const PatternList& result() const;

signals:
    void finished();

protected:
    IsotopicPattern* pattern;
    PatternList resultList;
};

#endif // ISOTOPICPATTERNWORKER_H
