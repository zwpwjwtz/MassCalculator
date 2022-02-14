#ifndef FRAMEISOTOPICPATTERNLIST_H
#define FRAMEISOTOPICPATTERNLIST_H

#include <QFrame>
#include <QStandardItemModel>
#include "calculator/chemicalcomposition.h"


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

protected:
    void changeEvent(QEvent* e);

private:
    Ui::FrameIsotopicPatternList *ui;
    ChemicalComposition currentComposition;
    QStandardItemModel modelList;
    double massShift;
};

#endif // FRAMEISOTOPICPATTERNLIST_H
