
QT       += core gui network xml

contains(QT_MAJOR_VERSION, 5) {
    message("Qt version >= 5.0")
    QT += widgets
    DEFINES += QT_5_0
} else {
    message("Qt version < 5.0")
}

TARGET = yle-downloader-gui
TEMPLATE = app

unix {
    target.path = /usr/bin
    icon.files = icons/yle-downloader-gui.svg
    icon.path = /usr/share/pixmaps
    desktopFile.files = yle-downloader-gui.desktop
    desktopFile.path = /usr/share/applications
    INSTALLS += target icon desktopFile
}

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
