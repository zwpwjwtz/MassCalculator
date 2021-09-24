#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>


class CompositionSelector;
class FormulaGeneratorWorker;
class QProgressBar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent* e);

private:
    Ui::MainWindow *ui;
    double lastSearchedMass;
    QProgressBar* progressBar;
    CompositionSelector* compositionList;
    FormulaGeneratorWorker* formulaGenerator;

    void showAllowedElementRanges();

private slots:
    void onCompositionSelectorFinished();
    void onFormulaGeneratorFinished();

    void on_tabWidget_currentChanged(int index);
    void on_textInputFormula_returnPressed();
    void on_textInputMass_returnPressed();
    void on_buttonGetMass_clicked();
    void on_buttonGetFormula_clicked();
    void on_buttonAllowedElement_clicked();
};

#endif // MAINWINDOW_H
