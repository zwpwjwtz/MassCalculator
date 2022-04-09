#include <QMouseEvent>
#include <QMenu>
#include <QClipboard>
#include "compositiontemplatelist.h"
#include "ui_compositiontemplatelist.h"
#include "calculator/atomname.h"

#define MC_WIDGET_COMPTEMP_MENU_COPY    1
#define MC_WIDGET_COMPTEMP_MENU_REMOVE  2


CompositionTemplateList::CompositionTemplateList(QWidget *parent) :
    QComboBox(parent),
    ui(new Ui::CompositionTemplateList)
{
    maxCount = 32;
    contextMenu = nullptr;

    ui->setupUi(this);
}

CompositionTemplateList::~CompositionTemplateList()
{
    delete ui;
}

void CompositionTemplateList::changeEvent(QEvent *e)
{
    QComboBox::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
}

void CompositionTemplateList::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        if (!contextMenu)
        {
            contextMenu = new QMenu(this);
            QAction* newAction = new QAction(tr("Copy"), contextMenu);
            newAction->setData(MC_WIDGET_COMPTEMP_MENU_COPY);
            contextMenu->addAction(newAction);
            newAction = new QAction(tr("Delete"), contextMenu);
            newAction->setData(MC_WIDGET_COMPTEMP_MENU_REMOVE);
            contextMenu->addAction(newAction);
            connect(contextMenu, SIGNAL(triggered(QAction*)),
                    this, SLOT(onContextMenuActionTriggered(QAction*)));
        }
        contextMenu->popup(QCursor::pos());
    }
    QComboBox::mousePressEvent(event);
}

QString CompositionTemplateList::compositionToString(
                                        const QList<ElementRange>& rangeList,
                                        const QString& remark)
{
    QString result;
    for (auto i=rangeList.cbegin(); i!=rangeList.cend(); i++)
    {
        result.append(
            QString("%1%2-%3 ").arg(QString::fromStdString(
                                        AtomName::abbreviation(i->atomNumber)))
                               .arg(QString::number(i->minCount))
                               .arg(QString::number(i->maxCount)));
    }
    if (remark.isEmpty())
        return result;
    else
        return QString("%1: %2").arg(remark).arg(result);
}

int CompositionTemplateList::maxCompositionCount() const
{
    return maxCount;
}

void CompositionTemplateList::setMaxCompositionCount(int maxCount)
{
    this->maxCount = maxCount >= 0 ? maxCount : 0;
}

QList<ElementRange> CompositionTemplateList::composition(int index) const
{
    if (index >= 0 && index < templateList.count())
        return templateList[index];
    else
        return QList<ElementRange>();
}

QString CompositionTemplateList::remark(int index) const
{

    if (index >= 0 && index < templateList.count())
        return remarkList[index];
    else
        return QString();
}

void
CompositionTemplateList::appendComposition(const QList<ElementRange>& rangeList,
                                           const QString& remark)
{
    int index = templateList.indexOf(rangeList);
    if (index >= 0)
    {
        remarkList[index] = remark;
        return;
    }

    templateList.push_back(rangeList);
    remarkList.push_back(remark);
    insertItem(count(), compositionToString(rangeList, remark));
}

void CompositionTemplateList::setCurrentComposition(
                                        const QList<ElementRange>& rangeList,
                                        const QString& remark,
                                        bool moveToTop)
{
    // See if the specified composition already exists in the list
    int index = templateList.indexOf(rangeList);
    if (index >= 0)
    {
        setCurrentIndex(index);

        // See if it is the (current) selected item
        if (index == 0 || !moveToTop)
        {
            // No adjustment of order needed; update the remark if needed
            if (remarkList[index] != remark)
                remarkList[index] = remark;
            setItemText(index, compositionToString(rangeList,
                                                   remarkList[index]));
            return;
        }
        else
        {
            removeItem(index);
            templateList.removeAt(index);
            remarkList.removeAt(index);
        }
    }

    // Remove the last item in the list if the list capacity is exceeded
    if (templateList.count() >= maxCount)
    {
        removeItem(count() - 1);
        templateList.pop_back();
        remarkList.pop_back();
    }

    // Prepend the item to the list
    templateList.push_front(rangeList);
    remarkList.push_front(remark);

    // Display a readable text for the specified composition to user
    insertItem(0, compositionToString(rangeList, remark));
    setCurrentIndex(0);
}

void CompositionTemplateList::onContextMenuActionTriggered(QAction* action)
{
    int index;
    switch (action->data().toInt())
    {
        case MC_WIDGET_COMPTEMP_MENU_COPY:
            QGuiApplication::clipboard()->setText(currentText());
            break;
        case MC_WIDGET_COMPTEMP_MENU_REMOVE:
            index = currentIndex();
            if (index >= 0)
            {
                removeItem(index);
                templateList.removeAt(index);
                remarkList.removeAt(index);
            }
            break;
        default:;
    }
}
