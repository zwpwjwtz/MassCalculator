#include <QLabel>
#include <QVBoxLayout>
#include "labeledtext.h"


LabeledText::LabeledText(QWidget *parent) : QLineEdit(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    setLayout(layout);

    label = new QLabel(this);
    layout->addWidget(label);
    layout->addStretch();
    label->setFocusPolicy(Qt::NoFocus);
    label->resize(width() / 3, height());
    setLabelAlignment(Qt::AlignRight);
}

LabeledText::~LabeledText()
{
    delete label;
}

Qt::Alignment LabeledText::labelAlignment()
{
    return labelPosition;
}

void LabeledText::setLabelAlignment(Qt::Alignment alignment)
{
    switch (alignment)
    {
        case Qt::AlignLeft:
        case Qt::AlignHCenter:
        case Qt::AlignRight:
            static_cast<QHBoxLayout*>(layout())->setAlignment(alignment);
            labelPosition = alignment;
            adjustTextMargin();
            break;
        default:;
    }
}

QString LabeledText::labelText()
{
    return label->text();
}

void LabeledText::setLabelText(const QString& text)
{
    label->setText(text);
    label->adjustSize();
    adjustTextMargin();
}

Qt::TextFormat LabeledText::labelTextFormat()
{
    return label->textFormat();
}

void LabeledText::setLabelTextFormat(Qt::TextFormat format)
{
    label->setTextFormat(format);
}

int LabeledText::labelWidth()
{
    return label->width();
}

void LabeledText::setLabelWidth(int width)
{
    if (width > this->width())
        width = this->width();
    label->resize(width, label->height());
    adjustTextMargin();
}

void LabeledText::adjustTextMargin()
{
    int labelWidth = label->width();
    switch (labelPosition)
    {
        case Qt::AlignLeft:
            setTextMargins(labelWidth, 0, 0, 0);
            break;
        case Qt::AlignRight:
            setTextMargins(0, 0, labelWidth, 0);
            break;
        default:;
    }
}
