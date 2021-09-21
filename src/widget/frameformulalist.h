#ifndef FRAMEFORMULALIST_H
#define FRAMEFORMULALIST_H

#include <QFrame>
#include "../calculator/formula.h"


class QStandardItemModel;
class QSortFilterProxyModel;

namespace Ui {
class FrameFormulaList;
}

class FrameFormulaList : public QFrame
{
    Q_OBJECT

public:
    explicit FrameFormulaList(QWidget *parent = nullptr);
    ~FrameFormulaList();

    void clear();
    void loadResult(const std::list<Formula>& formulaList,
                    double expectedMass = 0);

protected:
    void changeEvent(QEvent *e);

signals:
    void fieldListRequested();
    void showDetailsRequested(QString ID);

private:
    Ui::FrameFormulaList *ui;
    QStandardItemModel* modelResult;
    QSortFilterProxyModel* modelResultProxy;

    bool resultLoaded;
    QString lastSavePath;
    QString lastSavePathFilter;
    QList<int> listColumnWidth;
    QList<bool> columnSortAscending;

    void saveColumnWidth();
    void restoreColumnWidth();
    void setColumnHeader(int columnCount);

private slots:
    void onViewHeaderSearchResultClicked(int columnIndex);
    void on_buttonOpenLink_clicked();
    void on_buttonSave_clicked();
};

#endif // FRAMEFORMULALIST_H
