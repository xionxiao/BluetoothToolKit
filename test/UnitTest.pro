#-------------------------------------------------
#
# Project created by QtCreator 2017-04-08T17:16:09
#
#-------------------------------------------------

QT       += qml quick testlib bluetooth

QT       -= gui

TARGET = bluetoothtoolkit_test
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# DEFINES += QT_NO_DEBUG_OUTPUT

DEFINES += SRCDIR=\\\"$$PWD/\\\"
INCLUDEPATH += ../include
HEADERS += \
    ../include/device.h \
    ../include/devicemanager.h \
    ../include/service.h \
    ../include/utils.h \
    ../include/dfuservice.h \
    test_service.h \
    test_device.h \
    test_devicemanager.h \
    test_utils.h
SOURCES += \
    ../src/devicemanager.cpp \
    ../src/device.cpp \
    ../src/service.cpp \
    ../src/utils.cpp \
    ../src/dfuservice.cpp \
    test_deivce.cpp \
    test_service.cpp \
    test_main.cpp \
    test_devicemanager.cpp \
    test_utils.cpp

DISTFILES +=

RESOURCES += \
    qml.qrc

