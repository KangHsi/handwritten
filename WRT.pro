#-------------------------------------------------
#
# Project created by QtCreator 2016-08-20T13:25:28
#
#-------------------------------------------------

QT       += core gui
QT       += network
QT       -= gui
QT       += multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WRT
TEMPLATE = app


SOURCES += main.cpp\
        wrt_window.cpp \
    iconhelper.cpp \
    TabletCanvas.cpp \
    toolpalette.cpp \
    mydraw.cpp

HEADERS  += wrt_window.h \
    toolbox.h \
    iconhelper.h \
    TabletCanvas.h \
    globalDefs.h \
    ToolPalette.h \
    mydraw.h

FORMS    += wrt_window.ui


RESOURCES += \
    rc.qrc \
    rc.qrc

INCLUDEPATH += D:/ProgramFiles/opencv/build/include/opencv \
               D:/ProgramFiles/opencv/build/include/opencv2 \
               D:/ProgramFiles/opencv/build/include


CONFIG(debug,debug|release){
    LIBS += D:\ProgramFiles\opencv\build\x64\vc12\lib\opencv_world310d.lib \

}
CONFIG(release,debug|release){
    LIBS += D:\ProgramFiles\opencv\build\x64\vc12\lib\opencv_world310.lib \

}

DISTFILES += \
    image/check_back.png
