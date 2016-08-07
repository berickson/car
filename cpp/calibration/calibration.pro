QT += core
QT -= gui

CONFIG += c++11
CONFIG += c++14
CONFIG += thread

TARGET = calibration
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    calibration.cpp

unix|win32: LIBS += -L/usr/local/lib -lncurses -lwiringPi -lopencv_core  -lopencv_imgcodecs -lopencv_videoio -lopencv_highgui -lopencv_imgproc -lopencv_features2d -lopencv_xfeatures2d -lopencv_calib3d


#include "opencv2/core.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
