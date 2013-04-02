#-------------------------------------------------
#
# Project created by QtCreator 2013-03-05T23:56:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FIT0201ANISUTINA_Plotter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cassiniwidget.cpp \
    cassinithread.cpp \
    curvewidget.cpp \
    curvethread.cpp

HEADERS  += mainwindow.h \
    cassiniwidget.h \
    cassinithread.h \
    curvewidget.h \
    curvethread.h

FORMS    += mainwindow.ui \
    cassiniwidget.ui \
    curvewidget.ui
