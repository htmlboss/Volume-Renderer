QT += core widgets

CONFIG += c++17

TARGET = ModelViewer
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    transform3d.cpp \
    oglwidget.cpp \
    mainwindow.cpp \
    input.cpp \
    camera.cpp \
    mainwindow.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    vertex.h \
    transform3d.h \
    oglwidget.h \
    mainwindow.h \
    input.h \
    camera.h \
    mainwindow.h

RESOURCES += \
    qdarkstyle/style.qrc \
    shaders.qrc \
    res/resources.qrc

DISTFILES += \
    shaders/basic.vert \
    shaders/basic.frag \
    shaders/backface.vert \
    shaders/backface.frag \
    shaders/raycast.vert \
    shaders/raycast.frag

FORMS += \
    mainwindow.ui
