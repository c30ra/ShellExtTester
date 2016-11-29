#-------------------------------------------------
#
# Project created by QtCreator 2015-04-03T11:46:35
#
#-------------------------------------------------

QT       += core gui winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IThumbnailTester
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    thumbnailrender.cpp

HEADERS  += mainwindow.h \
    thumbnailrender.h

FORMS    += mainwindow.ui

LIBS += -lgdi32 \
        -luser32 \
        -lshell32 \
        -lole32
