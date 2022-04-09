#include "compositionselector.h"
#include "ui_compositionselector.h"
#include "../calculator/atomname.h"

#define MC_COMPSELECTOR_ROLE_ATOMNUM    Qt::UserRole+2


CompositionSelector::CompositionSelector(QWidget *parent) :
    QFrame (parent),
    ui(new Ui::CompositionSelector)
{
    // Set table layout
    modelElement.setColumnCount(4);

    // Add default elements
    addElement(6, 1, 5);
    addElement(1, 5, 10);
    addElement(8, 0, 3);

    ui->setupUi(this);
    ui->viewElement->setModel(&modelElement);
    ui->viewElement->setItemDelegate(&delegate);
}

CompositionSelector::~CompositionSelector()
{
    delete ui;
}

void CompositionSelector::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            modelElement.setHorizontalHeaderLabels({tr("Element"),
                                                    tr("Min"),
                                                    tr("Max"),
                                                    tr("Enabled")});
            break;
        default:
            break;
    }
}

void CompositionSelector::addElement(int atomNumber,
                                     int minCount, int maxCount,
                                     bool enable)
{
    QList<QStandardItem*> newRow;
    for (int i=0; i<4; i++)
        newRow.push_back(new QStandardItem);
    newRow[0]->setData(QString::fromStdString(
                                   AtomName::abbreviation(atomNumber)),
                       Qt::DisplayRole);
    newRow[0]->setData(atomNumber, MC_COMPSELECTOR_ROLE_ATOMNUM);
    newRow[1]->setData(minCount, Qt::DisplayRole | Qt::EditRole);
    newRow[2]->setData(maxCount, Qt::DisplayRole | Qt::EditRole);
    newRow[3]->setCheckable(enable);
    newRow[3]->setCheckState(Qt::Checked);
    newRow[3]->setEditable(false);
    modelElement.appendRow(newRow);
}

void CompositionSelector::removeElement(int atomNumber)
{
    for (int i=modelElement.rowCount() - 1; i>=0; i--)
    {
        if (atomNumber ==
            modelElement.item(i, 0)->data(MC_COMPSELECTOR_ROLE_ATOMNUM).toInt())
            modelElement.removeRow(i);
    }
}

void CompositionSelector::clear()
{
    modelElement.setRowCount(0);
}

QList<ElementRange> CompositionSelector::getElementRanges() const
{
    ElementRange range;
    QList<ElementRange> elementRangeList;
    for (int i=0; i<modelElement.rowCount(); i++)
    {
        if (modelElement.item(i, 3)->checkState() != Qt::Checked)
            continue;
        range.atomNumber =
            modelElement.item(i, 0)->data(MC_COMPSELECTOR_ROLE_ATOMNUM).toInt();
        range.minCount = modelElement.item(i, 1)->text().toInt();
        range.maxCount = modelElement.item(i, 2)->text().toInt();
        elementRangeList.push_back(range);
    }
    return elementRangeList;
}

void CompositionSelector::setElementRanges(const QList<ElementRange>& rangeList)
{
    clear();
    for (auto range=rangeList.cbegin(); range!=rangeList.cend(); range++)
        addElement(range->atomNumber, range->minCount, range->maxCount);
}

QString CompositionSelector::getRemark() const
{
    return ui->textRemark->text();
}

void CompositionSelector::setRemark(const QString& remark)
{
    ui->textRemark->setText(remark);
}

void CompositionSelector::on_buttonAdd_clicked()
{
    addElement(1, 1, 10);
}

void CompositionSelector::on_buttonRemove_clicked()
{
    for (int i=modelElement.rowCount() - 1; i>=0; i--)
    {
        if (ui->viewElement->selectionModel()->isRowSelected(i, QModelIndex()))
            modelElement.removeRow(i);
    }
}

void CompositionSelector::on_buttonOK_clicked()
{
    emit finished();
}
