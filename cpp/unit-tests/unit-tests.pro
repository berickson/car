GOOGLETEST_DIR = /home/brian/googletest

!isEmpty(GOOGLETEST_DIR): {
    GTEST_SRCDIR = $$GOOGLETEST_DIR/googletest
    GMOCK_SRCDIR = $$GOOGLETEST_DIR/googlemock
}

requires(exists($$GTEST_SRCDIR):exists($$GMOCK_SRCDIR))

!exists($$GOOGLETEST_DIR):message("No googletest src dir found - set GOOGLETEST_DIR to enable.")


INCLUDEPATH += \
    $$GTEST_SRCDIR \
    $$GTEST_SRCDIR/include \
    $$GMOCK_SRCDIR \
    $$GMOCK_SRCDIR/include

SOURCES += \
    $$GTEST_SRCDIR/src/gtest-all.cc \
    $$GMOCK_SRCDIR/src/gmock-all.cc \
    ../dynamics.cpp \
    ../geometry.cpp \
    ../ackerman.cpp \
    ../camera.cpp \
    ../car.cpp \
    ../car_menu.cpp \
    ../car_ui.cpp \
    ../config.cpp \
    ../console_menu.cpp \
    ../driver.cpp \
    ../fake_car.cpp \
    ../file_names.cpp \
    ../frame_grabber.cpp \
    ../kalman.cpp \
    ../menu.cpp \
    ../pid.cpp \
    ../route.cpp \
    ../run_settings.cpp \
    ../speedometer.cpp \
    ../string_utils.cpp \
    ../system.cpp \
    ../tracker.cpp \
    ../usb.cpp \
    ../work_queue.cpp \
    ../logger.cpp \
    ../lookup_table.cpp \
    ../pi_buttons.cpp \
    ackerman_test.cpp


LIBS += -L/usr/local/lib -lncurses -lwiringPi -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_xfeatures2d -lopencv_calib3d

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt


SOURCES +=     main.cpp
