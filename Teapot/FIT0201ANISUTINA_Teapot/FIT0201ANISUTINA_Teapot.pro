#-------------------------------------------------
#
# Project created by QtCreator 2013-04-02T19:06:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FIT0201ANISUTINA_Teapot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    calculator.cpp \
    teapotprojectorwidget.cpp

HEADERS  += mainwindow.h \
    calculator.h \
    teapotprojectorwidget.h

FORMS    += mainwindow.ui \
    teapotprojectorwidget.ui

RESOURCES += \
    3dmodels.qrc
