#ifndef DIALOGFORMULALISTFILTER_H
#define DIALOGFORMULALISTFILTER_H

#include <QDialog>


namespace Ui {
class DialogFormulaListFilter;
}

class DialogFormulaListFilter : public QDialog
{
    Q_OBJECT

public:
    explicit DialogFormulaListFilter(QWidget *parent = nullptr);
    ~DialogFormulaListFilter();

    bool isActivated() const;
    bool filterSaturationDegree() const;

    void setSaturationDegreeRange(double min, double max);
    double saturationDegreeMin() const;
    double saturationDegreeMax() const;

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DialogFormulaListFilter *ui;

private slots:
    void on_checkUnsaturation_stateChanged(int arg1);
};

#endif // DIALOGFORMULALISTFILTER_H
