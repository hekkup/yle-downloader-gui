
CONFIG   += link_pkgconfig

QT       += core gui

TARGET = yle-downloader-gui
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp

HEADERS  += MainWindow.h

FORMS    += MainWindow.ui

PKGCONFIG += json
