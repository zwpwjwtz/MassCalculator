#ifndef FRAMEISOTOPICPATTERNLIST_H
#define FRAMEISOTOPICPATTERNLIST_H

#include <QFrame>
#include <QStandardItemModel>
#include "calculator/chemicalcomposition.h"


class IsotopicPatternWorker;

namespace Ui {
class FrameIsotopicPatternList;
}

class FrameIsotopicPatternList : public QFrame
{
    Q_OBJECT

public:
    explicit FrameIsotopicPatternList(QWidget* parent = nullptr);
    ~FrameIsotopicPatternList();

    void clear();
    void setComposition(const ChemicalComposition& composition);
    void setMassShift(double delta);
    void setBinningWidth(double width);
    void setMaxLength(int maxLength);
    void setMassPrecision(int digits);

protected:
    void changeEvent(QEvent* e);
    void resizeEvent(QResizeEvent* e);

private:
    Ui::FrameIsotopicPatternList *ui;
    QStandardItemModel modelList;
    ChemicalComposition lastComposition;
    IsotopicPatternWorker* patternGenerator;
    std::vector<std::pair<double, double>> currentPattern;
    double massShift;
    int massPrecision;

private slots:
    void on_buttonCancelUpdaing_clicked();
    void onGeneratorFinished();
};

#endif // FRAMEISOTOPICPATTERNLIST_H
