CONFIG += c++11 c++14

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
    ../config.cpp \
    ../driver.cpp \
    ../fake_car.cpp \
    ../file_names.cpp \
    ../frame_grabber.cpp \
    ../kalman.cpp \
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
    ../socket_server.cpp \
    ../stereo_camera.cpp \
    ../image_utils.cpp \
    ackerman_test.cpp \
    work_queue_test.cpp \
    async_buf_tests.cpp \
    route_tests.cpp \
    geometry_tests.cpp


LIBS += -L/usr/local/lib -lopencv_core -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_xfeatures2d -lopencv_calib3d

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += thread
CONFIG -= qt


SOURCES +=     main.cpp

HEADERS +=
