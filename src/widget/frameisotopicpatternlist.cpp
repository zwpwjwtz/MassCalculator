#include "frameisotopicpatternlist.h"
#include "ui_frameisotopicpatternlist.h"
#include "thread/isotopicpatternworker.h"


FrameIsotopicPatternList::FrameIsotopicPatternList(QWidget* parent) :
    QFrame(parent),
    ui(new Ui::FrameIsotopicPatternList)
{
    massShift = 0;
    modelList.setColumnCount(3);
    modelList.setHorizontalHeaderLabels({"Isotopic Mass",
                                         "Normalized Intensity",
                                         "Relative Intensity"});

    patternGenerator = new IsotopicPatternWorker(this);

    ui->setupUi(this);
    ui->tableView->setModel(&modelList);
    ui->tableView->horizontalHeader()
                 ->setSectionResizeMode(QHeaderView::Stretch);
    ui->frameUpdating->setVisible(false);

    connect(patternGenerator, SIGNAL(finished()),
            this, SLOT(onGeneratorFinished()));
}

FrameIsotopicPatternList::~FrameIsotopicPatternList()
{
    delete ui;
    if (patternGenerator->isRunning())
    {
        patternGenerator->terminate();
        patternGenerator->wait();
    }
    delete patternGenerator;
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

void FrameIsotopicPatternList::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e)
    int h = height(), w = width();
    ui->tableView->resize(w - 20, h - 20);
    ui->frameUpdating->move(ui->frameUpdating->x(), int(h * 0.3));
    ui->frameUpdating->resize(w - 22, int(h * 0.5));
}

void FrameIsotopicPatternList::clear()
{
    modelList.setRowCount(0);
    patternGenerator->clear();
}

void
FrameIsotopicPatternList::setComposition(const ChemicalComposition& composition)
{
    if (patternGenerator->isRunning())
    {
        // Force terminating the calculation thread
        patternGenerator->terminate();
        patternGenerator->wait();
    }

    if (lastComposition != composition)
    {
        // Composition has ben updated; redo the calculation
        patternGenerator->setComposition(composition);
        patternGenerator->start();
        ui->frameUpdating->setVisible(true);
    }
    else
        onGeneratorFinished();
}

void FrameIsotopicPatternList::setMassShift(double delta)
{
    massShift = delta;
}

void FrameIsotopicPatternList::on_buttonCancelUpdaing_clicked()
{
    if (patternGenerator->isRunning())
    {
        // Force terminating the calculation thread
        patternGenerator->terminate();
        patternGenerator->wait();
    }
    ui->frameUpdating->setVisible(false);
}

void FrameIsotopicPatternList::onGeneratorFinished()
{
    // Load the calculation result
    currentPattern = patternGenerator->result();
    lastComposition = patternGenerator->composition();

    // Convert normalized abundance to relative abundance (max-peak normalized)
    size_t i;
    double maxAbundance = 0;
    for (i=0; i<currentPattern.size(); i++)
    {
        if (currentPattern[i].second > maxAbundance)
            maxAbundance = currentPattern[i].second;
    }
    std::vector<double> relativeAbundance(currentPattern.size(), 0);
    if (maxAbundance > 0)
    {
        maxAbundance *= 0.01;
        for (i=0; i<currentPattern.size(); i++)
            relativeAbundance[i] = currentPattern[i].second / maxAbundance;
    }

    modelList.setRowCount(0);
    for (i=0; i<currentPattern.size(); i++)
        modelList.appendRow(
            {new QStandardItem(QString::number(currentPattern[i].first +
                                               massShift, 'f', 6)),
             new QStandardItem(QString::number(currentPattern[i].second)),
             new QStandardItem(QString::number(relativeAbundance[i]))});

    ui->tableView->horizontalHeader()
                 ->setSectionResizeMode(QHeaderView::Interactive);

    ui->frameUpdating->setVisible(false);
}
