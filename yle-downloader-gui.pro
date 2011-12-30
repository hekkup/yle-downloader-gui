
QT       += core gui network xml webkit

TARGET = yle-downloader-gui
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Downloader.cpp \
    ProgressParser.cpp \
    required_qt_translations.cpp \
    ProcessUtils.cpp \
    UpdateChecker.cpp

HEADERS  += MainWindow.h \
    Downloader.h \
    ProgressParser.h \
    ProcessUtils.h \
    UpdateChecker.h \
    config.h

FORMS    += MainWindow.ui

TRANSLATIONS = fi.ts

RESOURCES = \
    resources.qrc

RC_FILE = windows-resources.rc

OTHER_FILES += \
    windows-resources.rc \
    README.md \
    "Tietoa ohjelmasta.txt" \
    update.xml
