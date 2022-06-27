#-------------------------------------------------
#
# Project created by QtCreator 2022-05-17T15:50:06
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BoneMerge
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += main.cpp\
        mainwindow.cpp \
    filemanager.cpp \
    merge.cpp \
    mergeexception.cpp

HEADERS  += mainwindow.h \
    filemanager.h \
    merge.h \
    mergeexception.h

FORMS    += mainwindow.ui

INCLUDEPATH += C:\opencv\build\include

LIBS += C:\opencv\opencv-build\bin\libopencv_core320.dll
LIBS += C:\opencv\opencv-build\bin\libopencv_highgui320.dll
LIBS += C:\opencv\opencv-build\bin\libopencv_imgcodecs320.dll
LIBS += C:\opencv\opencv-build\bin\libopencv_imgproc320.dll
LIBS += C:\opencv\opencv-build\bin\libopencv_features2d320.dll
LIBS += C:\opencv\opencv-build\bin\libopencv_calib3d320.dll
