#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>


class CompositionSelector;

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

private:
    Ui::MainWindow *ui;
    QStandardItemModel formulaList;
    CompositionSelector* compositionList;

private slots:
    void on_tabWidget_currentChanged(int index);
    void on_textInputFormula_returnPressed();
    void on_textInputMass_returnPressed();
    void on_buttonGetMass_clicked();
    void on_buttonGetFormula_clicked();
    void on_buttonAllowedElement_clicked();
};

#endif // MAINWINDOW_H
