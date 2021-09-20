#ifndef COMPOSITIONSELECTOR_H
#define COMPOSITIONSELECTOR_H

#include <QFrame>
#include <QStandardItemModel>
#include "calculator/elementrange.h"
#include "compositionselectordelegate.h"


namespace Ui {
class CompositionSelector;
}

class CompositionSelector : public QFrame
{
    Q_OBJECT

public:
    explicit CompositionSelector(QWidget* parent = nullptr);
    ~CompositionSelector();

    void addElement(int atomNumber,
                    int minCount, int maxCount,
                    bool enable = true);
    QList<ElementRange> getElementRanges() const;
    void removeElement(int atomNumber);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CompositionSelector *ui;
    QStandardItemModel modelElement;
    CompositionSelectorDelegate delegate;

private slots:
    void on_buttonAdd_clicked();
    void on_buttonRemove_clicked();
};

#endif // COMPOSITIONSELECTOR_H
