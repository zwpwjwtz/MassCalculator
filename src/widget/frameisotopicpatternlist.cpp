#include "frameisotopicpatternlist.h"
#include "ui_frameisotopicpatternlist.h"
#include "calculator/isotopicpattern.h"


FrameIsotopicPatternList::FrameIsotopicPatternList(QWidget* parent) :
    QFrame(parent),
    ui(new Ui::FrameIsotopicPatternList)
{
    massShift = 0;
    modelList.setColumnCount(2);
    modelList.setHorizontalHeaderLabels({"Isotopic Mass",
                                         "Relative Intensity"});

    ui->setupUi(this);
    ui->tableView->setModel(&modelList);
    ui->tableView->horizontalHeader()
                 ->setSectionResizeMode(QHeaderView::Stretch);
}

FrameIsotopicPatternList::~FrameIsotopicPatternList()
{
    delete ui;
}

void FrameIsotopicPatternList::changeEvent(QEvent* e)
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

void FrameIsotopicPatternList::clear()
{
    modelList.setRowCount(0);
    currentComposition.clear();
}

void
FrameIsotopicPatternList::setComposition(const ChemicalComposition& composition)
{
    if (currentComposition == composition)
        return;
    else
        currentComposition = composition;

    IsotopicPattern pattern;
    if (!pattern.setComposition(currentComposition))
        return;

    std::vector<std::pair<double, double>> patternList = pattern.distribution();
    modelList.setRowCount(0);
    for (auto i=patternList.cbegin(); i!=patternList.cend(); i++)
        modelList.appendRow(
            {new QStandardItem(QString::number(i->first + massShift)),
             new QStandardItem(QString::number(i->second))});

    ui->tableView->horizontalHeader()
                 ->setSectionResizeMode(QHeaderView::Interactive);
}

void FrameIsotopicPatternList::setMassShift(double delta)
{
    massShift = delta;
}
