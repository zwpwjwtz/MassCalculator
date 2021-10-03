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
    void setCharge(int charge);
    Formula modification() const;
    void setModification(const Formula& formula,
                         const QString& text = QString());

signals:
    void chargeChanged();
    void modificationChanged();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::FrameFormulaModification *ui;
    QList<Formula> formulaList;

    void loadDefaultModification();
};

#endif // FRAMEFORMULAMODIFICATION_H
