#include "formabout.h"
#include "ui_formabout.h"

FormAbout::FormAbout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAbout)
{
    ui->setupUi(this);
}

FormAbout::~FormAbout()
{
    delete ui;
}

void FormAbout::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        default:
            break;
    }
    ui->labelVersion->setText(QString(tr("Ver: %1")).arg(APP_VERSION));
    ui->textAbout->setPlainText(
                tr("MassCalculator - A simple tool for calculating molecular weight.\n\n"
                   "This program is a free software.\n\n"
                   "You can redistribute it and/or modify it under the terms of "
                   "the GNU General Public License as published by "
                   "the Free Software Foundation; either version 3 of the License, "
                   "or (at your option) any later version.\n\n"
                   "This program is distributed in the hope that it will be useful, but WITHOUT "
                   "ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or "
                   "FITNESS FOR A PARTICULAR PURPOSE. See the GNU Library General Public License "
                   "for more details.\n"));
}
