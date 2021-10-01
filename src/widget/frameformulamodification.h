#ifndef FRAMEFORMULAMODIFICATION_H
#define FRAMEFORMULAMODIFICATION_H

#include <QFrame>
#include "../calculator/formula.h"


namespace Ui {
class FrameFormulaModification;
}

class FrameFormulaModification : public QFrame
{
    Q_OBJECT

public:
    explicit FrameFormulaModification(QWidget *parent = nullptr);
    ~FrameFormulaModification();

    int charge() const;
    Formula modification() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FrameFormulaModification *ui;
    QList<Formula> formulaList;

    void loadDefaultModification();
};

#endif // FRAMEFORMULAMODIFICATION_H
