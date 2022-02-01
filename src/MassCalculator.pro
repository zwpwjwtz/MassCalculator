#-------------------------------------------------
#
# Project created by QtCreator 2021-09-18T20:43:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MassCalculator
TEMPLATE = app

VERSION = 1.1.0

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS APP_VERSION=\\\"$$VERSION\\\"

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
        mainwindow.cpp \
    calculator/atommass.cpp \
    calculator/atomname.cpp \
    calculator/chemicalcomposition.cpp \
    calculator/formulagenerator.cpp \
    calculator/formula.cpp \
    widget/compositionselector.cpp \
    widget/compositionselectordelegate.cpp \
    widget/frameformulalist.cpp \
    thread/formulageneratorworker.cpp \
    thread/formulageneratorbatchworker.cpp \
    widget/frameformulamodification.cpp \
    calculator/formula_adduct.cpp \
    config.cpp \
    widget/labeledtext.cpp \
    component/formformulatomass.cpp \
    component/formmasstoformula.cpp \
    component/formabout.cpp

HEADERS += \
        mainwindow.h \
    atom/atomic_weight.h \
    atom/atomic_abbreviation.h \
    calculator/atommass.h \
    calculator/atomname.h \
    calculator/chemicalcomposition.h \
    calculator/formulagenerator.h \
    calculator/formulagenerator_p.h \
    calculator/formula.h \
    widget/compositionselector.h \
    calculator/elementrange.h \
    widget/compositionselectordelegate.h \
    atom/atomic_weight_average.h \
    widget/frameformulalist.h \
    thread/formulageneratorworker.h \
    thread/formulageneratorbatchworker.h \
    widget/frameformulamodification.h \
    calculator/formula_adduct.h \
    global.h \
    config.h \
    widget/labeledtext.h \
    component/formformulatomass.h \
    component/formmasstoformula.h \
    component/formabout.h

FORMS += \
        mainwindow.ui \
    widget/compositionselector.ui \
    widget/frameformulalist.ui \
    widget/frameformulamodification.ui \
    component/formformulatomass.ui \
    component/formmasstoformula.ui \
    component/formabout.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

TRANSLATIONS += \
    translation/MassCalculator_zh_CN.ts

include(translation.pri)
