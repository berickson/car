TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
#CONFIG -= qt
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
    system.cpp \
    geometry.cpp \
    route.cpp \
    string_utils.cpp \
    driver.cpp \
    lookup_table.cpp \
    run_settings.cpp \
    camera.cpp \
    tracker.cpp \
    file_names.cpp \
    frame_grabber.cpp \
    logger.cpp \
    speedometer.cpp \
    pid.cpp \
    kalman.cpp \
    socket_server.cpp \
    car_controller.cpp

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
    ends_with.h \
    fake_car.h \
    menu.h \
    system.h \
    pi_buttons.h \
    route.h \
    string_utils.h \
    driver.h \
    car_ui.h \
    lookup_table.h \
    run_settings.h \
    camera.h \
    tracker.h \
    file_names.h \
    frame_grabber.h \
    logger.h \
    speedometer.h \
    pid.h \
    kalman.h \
    async_buf.h \
    socket_server.h




unix|win32: LIBS += -L/usr/local/lib -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_xfeatures2d -lopencv_calib3d

DISTFILES += \
    README.md

#TEMPLATE = subdirs

SUBDIRS += src

CONFIG(debug, debug|release) {
    SUBDIRS += tests
}
