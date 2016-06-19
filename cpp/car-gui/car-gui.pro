#-------------------------------------------------
#
# Project created by QtCreator 2016-06-07T10:39:02
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = car-gui
TEMPLATE = app


SOURCES += main.cpp\
        main_window.cpp \
    ../ackerman.cpp \
    ../car.cpp \
    ../config.cpp \
    ../dynamics.cpp \
    ../fake_car.cpp \
    ../usb.cpp \
    ../work_queue.cpp

HEADERS  += main_window.h \
    ../ackerman.h \
    ../car.h \
    ../config.h \
    ../dynamics.h \
    ../ends_with.h \
    ../fake_car.h \
    ../geometry.h \
    ../glob_util.h \
    ../split.h \
    ../trim.h \
    ../usb.h \
    ../work_queue.h

FORMS    += main_window.ui
