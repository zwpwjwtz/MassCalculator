#include <QLineEdit>
#include <QMessageBox>
#include "compositionselectordelegate.h"
#include "calculator/atomname.h"

#define MC_COMPSELECTOR_ROLE_ATOMNUM    Qt::UserRole+2


CompositionSelectorDelegate::CompositionSelectorDelegate(QObject* parent)
    : QItemDelegate(parent)
{}

void CompositionSelectorDelegate::setEditorData(QWidget* editor,
                                                const QModelIndex& index) const
{
    if (index.column() != 0)
        return QItemDelegate::setEditorData(editor, index);

    auto textEditor = dynamic_cast<QLineEdit*>(editor);
    if (textEditor == nullptr)
    {
        // Not a text editor; ignore it
        return QItemDelegate::setEditorData(editor, index);
    }

    textEditor->setText(
                QString::fromStdString(AtomName::abbreviation(
                    index.data(MC_COMPSELECTOR_ROLE_ATOMNUM).toInt())));
}

void CompositionSelectorDelegate::setModelData(QWidget* editor,
                                               QAbstractItemModel* model,
                                               const QModelIndex& index) const
{
    if (index.column() != 0)
        return QItemDelegate::setModelData(editor, model, index);

    auto textEditor = dynamic_cast<QLineEdit*>(editor);
    if (textEditor == nullptr)
    {
        // Not a text editor; ignore it
        return QItemDelegate::setModelData(editor, model, index);
    }

    QString input(textEditor->text().trimmed());
    int atomNumber = AtomName::numberFromAbbreviation(input.toStdString());
    if (atomNumber > 0)
    {
        model->setData(index, input, Qt::DisplayRole);
        model->setData(index, atomNumber, Qt::UserRole+2);
    }
    else
    {
        QMessageBox::warning(nullptr, "Invalid element name",
                             "The name that you input does not represent "
                             "any element. Please check and try again.");
    }
}
