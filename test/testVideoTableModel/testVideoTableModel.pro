#-------------------------------------------------
#
# Project created by QtCreator 2013-09-01T00:53:24
#
#-------------------------------------------------

QT       += testlib

#QT		 -= gui

TARGET = testVideoTableModel
CONFIG   += console testcase
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../..

HEADERS += \
	../../VideoTableModel.h \
	../../VideoInfo.h

SOURCES += testVideoTableModel.cpp \
	../../VideoTableModel.cpp \
	../../VideoInfo.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\" \
  UNIT_TEST

