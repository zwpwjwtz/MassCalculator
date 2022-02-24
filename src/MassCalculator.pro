#-------------------------------------------------
#
# Project created by QtCreator 2021-09-18T20:43:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MassCalculator
TEMPLATE = app

VERSION = 2.0.1

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
    widget/labeledtext.cpp \
    component/formformulatomass.cpp \
    component/formmasstoformula.cpp \
    component/formabout.cpp \
    atom/atomic_weight_isotopic.cpp \
    atom/atomic_weight.cpp \
    atom/atomic_weight_average.cpp \
    atom/atomic_abundance_isotopic.cpp \
    atom/atomic_abbreviation.cpp \
    calculator/atomabundance.cpp \
    calculator/isotopicpattern.cpp \
    widget/frameisotopicpatternlist.cpp \
    thread/isotopicpatternworker.cpp \
    component/config.cpp \
    component/framesettings.cpp \
    atom/atomic_valence.cpp \
    calculator/atomvalence.cpp \
    calculator/valence.cpp \
    widget/dialogformulalistfilter.cpp \
    widget/formulalistproxymodel.cpp

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
    widget/labeledtext.h \
    component/formformulatomass.h \
    component/formmasstoformula.h \
    component/formabout.h \
    atom/atomic_weight_isotopic.h \
    atom/atomic_abundance_isotopic.h \
    calculator/atomabundance.h \
    calculator/isotopicpattern.h \
    widget/frameisotopicpatternlist.h \
    calculator/isotopicpattern_p.h \
    thread/isotopicpatternworker.h \
    component/config.h \
    component/config_field.h \
    component/framesettings.h \
    atom/atomic_valence.h \
    calculator/atomvalence.h \
    calculator/valence.h \
    widget/dialogformulalistfilter.h \
    widget/formulalistproxymodel.h

FORMS += \
        mainwindow.ui \
    widget/compositionselector.ui \
    widget/frameformulalist.ui \
    widget/frameformulamodification.ui \
    component/formformulatomass.ui \
    component/formmasstoformula.ui \
    component/formabout.ui \
    widget/frameisotopicpatternlist.ui \
    component/framesettings.ui \
    widget/dialogformulalistfilter.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

RC_ICONS = icons/MassCalculator.ico

TRANSLATIONS += \
    translation/MassCalculator_zh_CN.ts

include(translation.pri)
