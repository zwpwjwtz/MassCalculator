#ifndef FORMFORMULATOMASS_H
#define FORMFORMULATOMASS_H

#include <QWidget>

namespace Ui {
class FormFormulaToMass;
}

class FormFormulaToMass : public QWidget
{
    Q_OBJECT

public:
    explicit FormFormulaToMass(QWidget *parent = nullptr);
    ~FormFormulaToMass();

    void loadPreferences();
    void savePreferences();

protected:
    void changeEvent(QEvent* e);
    void showEvent(QShowEvent* e);

private:
    Ui::FormFormulaToMass *ui;

private slots:
    void onFormulaModificationChanged();

    void on_textInputFormula_returnPressed();
    void on_buttonGetMass_clicked();
    void on_buttonModification_clicked();
};

#endif // FORMFORMULATOMASS_H
