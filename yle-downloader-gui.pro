
QT       += core gui network xml

TARGET = yle-downloader-gui
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    Downloader.cpp \
    ProgressParser.cpp \
    required_qt_translations.cpp \
    ProcessUtils.cpp \
    UpdateChecker.cpp \
    YlePassi.cpp \
    YlePassiDialog.cpp

HEADERS  += MainWindow.h \
    Downloader.h \
    ProgressParser.h \
    ProcessUtils.h \
    UpdateChecker.h \
    config.h \
    YlePassi.h \
    YlePassiDialog.h

FORMS    += MainWindow.ui \
    YlePassiDialog.ui

TRANSLATIONS = fi.ts

RESOURCES = \
    resources.qrc

RC_FILE = windows-resources.rc

OTHER_FILES += \
    windows-resources.rc \
    README.md \
    "Tietoa ohjelmasta.txt" \
    update.xml
