#ifndef COMPOSITIONSELECTORDELEGATE_H
#define COMPOSITIONSELECTORDELEGATE_H

#include <QItemDelegate>


class CompositionSelectorDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit CompositionSelectorDelegate(QObject* parent = nullptr);

    virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;
    virtual void setModelData(QWidget* editor,
                              QAbstractItemModel* model,
                              const QModelIndex& index) const;
};

#endif // COMPOSITIONSELECTORDELEGATE_H
