
QT      += core gui \
    widgets opengl
QT += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = box_1
TEMPLATE = app
LIBS += -lglut -lGLU
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    geometry.cpp \
    positions.cpp \
    mb22.cpp

HEADERS  += mainwindow.h \
    geometry.h \
    positions.h \
    mb22.h

RESOURCES   += resource.qrc \
    shaders.qrc
