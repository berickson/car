TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++14
CONFIG += thread
SOURCES += main.cpp \
    ackerman.cpp \
    usb.cpp \
    dynamics.cpp \
    car.cpp \
    config.cpp \
    work_queue.cpp \
    fake_car.cpp \
    menu.cpp \
    system.cpp \
    car_menu.cpp \
    console_menu.cpp \
    geometry.cpp \
    pi_buttons.cpp \
    route.cpp \
    string_utils.cpp \
    driver.cpp \
    car_ui.cpp \
    lookup_table.cpp

HEADERS += \
    geometry.h \
    ackerman.h \
    usb.h \
    work_queue.h \
    dynamics.h \
    split.h \
    car.h \
    config.h \
    trim.h \
    glob_util.h \
    ends_with.h \
    fake_car.h \
    menu.h \
    system.h \
    car_menu.h \
    console_menu.h \
    filenames.h \
    pi_buttons.h \
    route.h \
    string_utils.h \
    driver.h \
    car_ui.h \
    lookup_table.h




unix|win32: LIBS += -lncurses -lwiringPi
