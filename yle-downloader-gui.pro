
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
    SubtitleLanguage.cpp \
    ProgressBarDelegate.cpp \
    VideoTableModel.cpp \
    VideoInfo.cpp \
    VideoTableView.cpp

HEADERS  += MainWindow.h \
    Downloader.h \
    ProgressParser.h \
    ProcessUtils.h \
    UpdateChecker.h \
    config.h \
    SubtitleLanguage.h \
    ProgressBarDelegate.h \
    VideoTableModel.h \
    VideoInfo.h \
    VideoTableView.h

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
