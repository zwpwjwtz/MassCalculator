#ifndef FORMULALISTPROXYMODEL_H
#define FORMULALISTPROXYMODEL_H

#include <QSortFilterProxyModel>


class FormulaListProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FormulaListProxyModel(QObject *parent = nullptr);

    void clearRangeFilter(int column);
    void setRangeFilter(int column, double min, double max);
    void setRangeFilterActivated(int column, bool activated = true);

protected:
     bool filterAcceptsRow(int sourceRow,
                           const QModelIndex& sourceParent) const override;

private:
     QMap<int, bool> rangeFilterActivated;
     QMap<int, QPair<double, double>> rangeFilter;
};

#endif // FORMULALISTPROXYMODEL_H
