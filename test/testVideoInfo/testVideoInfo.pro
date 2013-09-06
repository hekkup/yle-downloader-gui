#-------------------------------------------------
#
# Project created by QtCreator 2013-09-01T16:47:52
#
#-------------------------------------------------

QT       += testlib

#QT       -= gui

TARGET = testVideoInfo
CONFIG   += console testcase
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../..

HEADERS += ../../VideoInfo.h

SOURCES += testVideoInfo.cpp \
  ../../VideoInfo.cpp

DEFINES += UNIT_TEST SRCDIR=\\\"$$PWD/\\\"
