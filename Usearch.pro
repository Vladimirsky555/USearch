#-------------------------------------------------
#
# Project created by QtCreator 2020-06-20T06:13:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 5.0
QMAKE_TARGET_COMPANY = BearWin
QMAKE_TARGET_PRODUCT = Search
QMAKE_TARGET_DESCRIPTION = Search
QMAKE_TARGET_COPYRIGHT = BearWin

TARGET = Usearch
TEMPLATE = app

CONFIG += resources_big

INCLUDEPATH += $${_PRO_FILE_PWD_}/src
win32: RC_ICONS = $$PWD/bin/images/app.ico


HEADERS += src/data/storage.h \
           src/data/catalog.h \
               src/data/bookitem.h \
           src/data/listitem.h \
    src/data/searchitem.h  \
    src/data/searchstorage.h \
           src/data/textitem.h \
           src/helpers/qregexphighlighter.h \
           src/helpers/searchitemsmaker.h \
           src/search/searchwindow.h \
           src/search/booksselector.h \
           src/search/catalogsselector.h

SOURCES += src/main.cpp\
    src/data/searchitem.cpp \
    src/data/searchstorage.cpp \
            src/data/storage.cpp \
           src/data/catalog.cpp \
           src/data/bookitem.cpp \
                   src/data/listitem.cpp \
           src/data/textitem.cpp \
           src/helpers/qregexphighlighter.cpp \
           src/helpers/searchitemsmaker.cpp \
           src/search/searchwindow.cpp \
           src/search/booksselector.cpp \
           src/search/catalogsselector.cpp

FORMS   += src/search/searchwindow.ui \
           src/search/catalogsselector.ui \
           src/search/booksselector.ui
           

RESOURCES += \
    bin/first_catalog.qrc \    
    bin/second_catalog.qrc \
    bin/third_catalog.qrc \
    bin/images.qrc \



DISTFILES +=
