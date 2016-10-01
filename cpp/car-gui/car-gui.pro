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
    ../work_queue.cpp \
    ../camera.cpp \
    ../car_menu.cpp \
    ../car_ui.cpp \
    ../console_menu.cpp \
    ../driver.cpp \
    ../geometry.cpp \
    ../lookup_table.cpp \
    ../menu.cpp \
    ../pi_buttons.cpp \
    ../route.cpp \
    ../run_settings.cpp \
    ../string_utils.cpp \
    ../system.cpp \
    ../tracker.cpp \
    route_window.cpp \
    ../file_names.cpp \
    ../frame_grabber.cpp \
    ../logger.cpp \
    ../speedometer.cpp \
    ../pid.cpp

HEADERS  += main_window.h \
    ../ackerman.h \
    ../car.h \
    ../config.h \
    ../dynamics.h \
    ../ends_with.h \
    ../fake_car.h \
    ../geometry.h \
    ../split.h \
    ../trim.h \
    ../usb.h \
    ../work_queue.h \
    route_window.h \
    ../frame_grabber.h \
    ../speedometer.h

images.path    = $${DESTDIR}/

QT += charts

FORMS    += main_window.ui \
    route_window.ui

unix|win32: LIBS += -L/usr/local/lib -lncurses -lwiringPi -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_xfeatures2d -lopencv_calib3d

RESOURCES += \
    ../resources.qrc


