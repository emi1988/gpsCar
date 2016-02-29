#-------------------------------------------------
#
# Project created by QtCreator 2016-02-24T17:24:21
#
#-------------------------------------------------

QT       += core gui
QT += network
QT += sql
#for qt5 or greater use :
#QT += serialport

CONFIG += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += serialport

TARGET = gpsCar
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dbManager.cpp

HEADERS  += mainwindow.h \
    defs.h \
    dbManager.h

FORMS    += mainwindow.ui
