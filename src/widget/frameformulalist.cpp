#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QUrl>
#include "frameformulalist.h"
#include "ui_frameformulalist.h"
#include "calculator/valence.h"
#include "formulalistproxymodel.h"
#include "dialogformulalistfilter.h"

#define MC_FORMULA_ELEMENT_ORDER   {"C"}

#define MC_FORMULA_LIST_COLUMN_DU  4

#define MC_FORMULA_OPENLINK_MAX    10

#define MC_FORMULA_FILE_SUFFIX_ALL "All (*)"
#define MC_FORMULA_FILE_SUFFIX_CSV "CSV (*.csv)"

#define MC_PUBCHEM_URL_PREFIX      "https://pubchem.ncbi.nlm.nih.gov/#query="


FrameFormulaList::FrameFormulaList(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FrameFormulaList)
{
    ui->setupUi(this);

    resultLoaded = false;
    dialogFilter = nullptr;
    modelResult = new QStandardItemModel(this);
    modelResultProxy = new FormulaListProxyModel(this);
    modelResultProxy->setSourceModel(modelResult);
    ui->viewSearchResult->setModel(modelResultProxy);
    ui->viewSearchResult->horizontalHeader()
                         ->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->viewSearchResult->horizontalHeader(),
            SIGNAL(sectionClicked(int)),
            this,
            SLOT(onViewHeaderSearchResultClicked(int)));
}

FrameFormulaList::~FrameFormulaList()
{
    if (dialogFilter)
        delete dialogFilter;
    delete ui;
    delete modelResultProxy;
    delete modelResult;
}

void FrameFormulaList::changeEvent(QEvent *e)
{
    QFrame::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void FrameFormulaList::clear()
{
    if (resultLoaded)
        saveColumnWidth();
    modelResult->clear();
    resultLoaded = false;
}

void FrameFormulaList::loadResult(const std::list<Formula>& formulaList,
                                  double expectedMass)
{
    if (resultLoaded)
        saveColumnWidth();

    modelResult->clear();
    columnSortAscending.clear();
    setColumnHeader(expectedMass > 0 ? 4 : 2);

    double formulaMass, massDiff;
    QList<QStandardItem*> newRow;
    std::vector<std::string> elementOrder(MC_FORMULA_ELEMENT_ORDER);
    for (auto j=formulaList.cbegin(); j!=formulaList.cend(); j++)
    {
        formulaMass = j->toMass();
        newRow << new QStandardItem(
                          QString::fromStdString(j->toString(elementOrder)))
               << new QStandardItem(QString::number(formulaMass, 'f', 6));
        newRow[1]->setData(formulaMass, Qt::DisplayRole);
        if (expectedMass > 0)
        {
            massDiff = expectedMass - formulaMass;
            newRow << new QStandardItem(QString::number(massDiff))
                   << new QStandardItem(
                              QString::number(massDiff / expectedMass  * 1E6))
                   << new QStandardItem(
                              QString::number(Valence::doubleBondIndex(*j)));
        }
        modelResult->appendRow(newRow);
        newRow.clear();
    }

    // Resize each column when first loaded
    if (!resultLoaded)
    {
        ui->viewSearchResult->resizeColumnsToContents();
        resultLoaded = true;
    }
    else
        restoreColumnWidth();
}

void FrameFormulaList::saveColumnWidth()
{
    listColumnWidth.clear();
    for (int i=0; i<modelResult->columnCount(); i++)
        listColumnWidth.push_back(ui->viewSearchResult->columnWidth(i));
}

void FrameFormulaList::restoreColumnWidth()
{
    for (int i=0; i<modelResult->columnCount(); i++)
    {
        if (i >= listColumnWidth.length())
            break;
        ui->viewSearchResult->setColumnWidth(i, listColumnWidth[i]);
    }
}

void FrameFormulaList::setColumnHeader(int columnCount)
{
    modelResult->setColumnCount(columnCount);
    modelResult->setHorizontalHeaderLabels({tr("Formula"),
                                            tr("Mono.Mass"),
                                            tr("Difference(Da)"),
                                            tr("Difference(ppm)"),
                                            tr("Unsaturation Degree")});
    ui->viewSearchResult->horizontalHeader()
                        ->setSectionResizeMode(QHeaderView::Interactive);
}

void FrameFormulaList::setFilterState(bool activated)
{
    ui->buttonFilter->setIcon(activated ?
                              QIcon(":/icons/view-filter-activated.png") :
                              QIcon(":/icons/view-filter.png"));
}

void FrameFormulaList::onViewHeaderSearchResultClicked(int columnIndex)
{
    if (columnSortAscending.length() < 1)
    {
        for (int i=0; i<modelResult->columnCount(); i++)
            columnSortAscending.push_back(false);
    }
    columnSortAscending[columnIndex] = !columnSortAscending[columnIndex];
    modelResult->sort(columnIndex,
                      columnSortAscending[columnIndex] ?
                          Qt::AscendingOrder : Qt::DescendingOrder);
}

void FrameFormulaList::on_buttonOpenLink_clicked()
{
    QModelIndexList selectedIndexes =
                    ui->viewSearchResult->selectionModel()->selectedIndexes();
    if (selectedIndexes.count() < 1)
        return;

    // Sort the selected indexes (by row as default),
    // and keep only one index for each line
    std::sort(selectedIndexes.begin(), selectedIndexes.end());
    QList<QModelIndex>::iterator i = selectedIndexes.begin() + 1;
    while (i != selectedIndexes.end())
    {
        if (i->row() == (i - 1)->row())
            i = selectedIndexes.erase(i);
        else
            i++;
    }

    QString recordID;
    for (i=selectedIndexes.begin(); i!=selectedIndexes.end(); i++)
    {
        if (i - selectedIndexes.begin() + 1 > MC_FORMULA_OPENLINK_MAX)
        {
            QMessageBox::warning(this, tr("Too many records selected"),
                                 QString(tr("You have selected %1 records.\n"
                                            "Only links to the first %2 records"
                                            " will be opened."))
                                        .arg(selectedIndexes.count())
                                        .arg(MC_FORMULA_OPENLINK_MAX));
            break;
        }

        recordID = modelResult->item(i->row(), 0)->text();
        if (!recordID.isEmpty())
        {
            QDesktopServices::openUrl(
                            recordID.prepend(MC_PUBCHEM_URL_PREFIX));
        }
    }
}

void FrameFormulaList::on_buttonSave_clicked()
{
    QString filter;
    filter.append(MC_FORMULA_FILE_SUFFIX_ALL).append(";;")
          .append(MC_FORMULA_FILE_SUFFIX_CSV).append(";;");

    if (lastSavePathFilter.isEmpty())
        lastSavePathFilter = MC_FORMULA_FILE_SUFFIX_CSV;
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save query result "
                                                       "as CSV file"),
                                                    lastSavePath,
                                                    filter,
                                                    &lastSavePathFilter);
    if (fileName.isEmpty())
        return;

    QFile f(fileName);
    if (!f.open(QFile::WriteOnly))
    {
        QMessageBox::critical(this, tr("Failed writing to file"),
                              QString(tr("Failed opening file %1 to write."))
                                     .arg(fileName));
        return;
    }

    // Write the table header
    int i, j;
    for (i=0; i<modelResultProxy->columnCount(); i++)
    {
        if (i > 0)
            f.putChar(',');
        f.write(modelResultProxy->headerData(i, Qt::Horizontal)
                                .toString().toLocal8Bit());
    }
    f.putChar('\n');

    int columnCount = modelResultProxy->columnCount();
    for (i=0; i<modelResultProxy->rowCount(); i++)
    {
        for (j=0; j<columnCount; j++)
        {
            if (j > 0)
                f.putChar(',');
            f.putChar('"');
            f.write(modelResultProxy->index(i, j)
                                    .data().toString().toLocal8Bit());
            f.putChar('"');
        }
        f.putChar('\n');
    }
    f.close();

    lastSavePath = QFileInfo(fileName).path();
}

void FrameFormulaList::on_buttonRemove_clicked()
{
    QModelIndexList selectedIndexes =
                    ui->viewSearchResult->selectionModel()->selectedIndexes();

    // Sort the selected indexes (by row as default)
    std::sort(selectedIndexes.begin(), selectedIndexes.end());

    for (int i = selectedIndexes.count() - 1; i >= 0; i--)
        modelResultProxy->removeRow(selectedIndexes[i].row());
}

void FrameFormulaList::on_buttonFilter_clicked()
{
    if (!dialogFilter)
        dialogFilter = new DialogFormulaListFilter(this);
    dialogFilter->exec();

    setFilterState(dialogFilter->isActivated());
    if (dialogFilter->filterSaturationDegree())
    {
        modelResultProxy->setRangeFilter(MC_FORMULA_LIST_COLUMN_DU,
                                dialogFilter->saturationDegreeMin(),
                                dialogFilter->saturationDegreeMax());
        modelResultProxy->setRangeFilterActivated(MC_FORMULA_LIST_COLUMN_DU,
                                                  true);
    }
    else
        modelResultProxy->setRangeFilterActivated(MC_FORMULA_LIST_COLUMN_DU,
                                                  false);
}
