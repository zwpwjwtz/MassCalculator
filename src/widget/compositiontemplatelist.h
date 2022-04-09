#ifndef COMPOSITIONTEMPLATELIST_H
#define COMPOSITIONTEMPLATELIST_H

#include <QComboBox>
#include "calculator/elementrange.h"


class QMenu;

namespace Ui {
class CompositionTemplateList;
}

class CompositionTemplateList : public QComboBox
{
    Q_OBJECT

public:
    explicit CompositionTemplateList(QWidget* parent = nullptr);
    ~CompositionTemplateList();

    int maxCompositionCount() const;
    void setMaxCompositionCount(int maxCount = 32);

    QList<ElementRange> composition(int index) const;
    QString remark(int index) const;
    void appendComposition(const QList<ElementRange>& rangeList,
                           const QString& remark = "");
    void setCurrentComposition(const QList<ElementRange>& rangeList,
                               const QString& remark = "",
                               bool moveToTop = false);

protected:
    void changeEvent(QEvent* e);
    void mousePressEvent(QMouseEvent* event);
    static QString compositionToString(const QList<ElementRange>& rangeList,
                                       const QString &remark);

private:
    Ui::CompositionTemplateList *ui;
    QList<QList<ElementRange>> templateList;
    QList<QString> remarkList;
    QMenu* contextMenu;
    int maxCount;

private slots:
    void onContextMenuActionTriggered(QAction* action);
};

#endif // COMPOSITIONTEMPLATELIST_H
