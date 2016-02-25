#-------------------------------------------------
#
# Project created by QtCreator 2016-02-24T17:24:21
#
#-------------------------------------------------

QT       += core gui
QT += network
#for qt5 or greater use :
#QT += serialport

CONFIG += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gpsCar
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    defs.h

FORMS    += mainwindow.ui
