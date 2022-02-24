#include "formulalistproxymodel.h"


FormulaListProxyModel::FormulaListProxyModel(QObject* parent)
    : QSortFilterProxyModel (parent)
{}

bool FormulaListProxyModel::filterAcceptsRow(int sourceRow,
                                        const QModelIndex& sourceParent) const
{
    Q_UNUSED(sourceParent)
    double value;
    int columnIndex;
    const QAbstractItemModel* model = sourceModel();
    for (auto i=rangeFilter.cbegin(); i!=rangeFilter.cend(); i++)
    {
        columnIndex = i.key();

        // See if the filter is activated
        if (!rangeFilterActivated.value(columnIndex))
            continue;

        // See if the data is in the specified range
        value = model->data(model->index(sourceRow, columnIndex)).toDouble();
        if (value < i.value().first || value > i.value().second)
            return false;
    }
    return true;
}

void FormulaListProxyModel::clearRangeFilter(int column)
{
    if (rangeFilter.find(column) == rangeFilter.cend())
        return;
    rangeFilter.remove(column);
    rangeFilterActivated.remove(column);
    invalidateFilter();
}

void FormulaListProxyModel::setRangeFilter(int column, double min, double max)
{
    if (column >= sourceModel()->columnCount())
        return;

    QPair<double, double> value(min, max);
    auto i = rangeFilter.find(column);
    if (i != rangeFilter.end())
    {
        if (i.value() == value)
            return;
        i.value() = value;
    }
    else
    {
        rangeFilterActivated.insert(column, true);
        rangeFilter.insert(column, value);
    }
    invalidateFilter();
}

void FormulaListProxyModel::setRangeFilterActivated(int column, bool activated)
{
    if (column >= sourceModel()->columnCount())
        return;

    auto i = rangeFilterActivated.find(column);
    if (i != rangeFilterActivated.end())
    {
        if (i.value() == activated)
            return;
        i.value() = activated;
    }
    else
    {
        if (rangeFilter.find(column) != rangeFilter.cend())
            rangeFilterActivated.insert(column, activated);
    }
    invalidateFilter();
}
