#ifndef LABELEDTEXT_H
#define LABELEDTEXT_H

#include <QLineEdit>


class QLabel;

class LabeledText : public QLineEdit
{
    Q_OBJECT
public:
    explicit LabeledText(QWidget* parent = nullptr);
    ~LabeledText();

public slots:
    Qt::Alignment labelAlignment();
    void setLabelAlignment(Qt::Alignment alignment);

    QString labelText();
    void setLabelText(const QString& text);

    Qt::TextFormat labelTextFormat();
    void setLabelTextFormat(Qt::TextFormat format);

    int labelWidth();
    void setLabelWidth(int width);

protected:
    QLabel* label;
    Qt::Alignment labelPosition;

    virtual void adjustTextMargin();
};

#endif // LABELEDTEXT_H
