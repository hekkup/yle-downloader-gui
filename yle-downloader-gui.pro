
CONFIG   += link_pkgconfig

QT       += core gui

TARGET = yle-downloader-gui
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Downloader.cpp \
    ProgressParser.cpp

HEADERS  += MainWindow.h \
    Downloader.h \
    ProgressParser.h

FORMS    += MainWindow.ui

PKGCONFIG += json
