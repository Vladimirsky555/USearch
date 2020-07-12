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

SOURCES += src/main.cpp\
            src/data/storage.cpp \
           src/data/catalog.cpp \           
           src/data/bookitem.cpp \
		   src/data/listitem.cpp \
           src/data/textitem.cpp \                   
           src/helpers/qregexphighlighter.cpp \
           src/search/searchwindow.cpp \
           src/search/booksselector.cpp \
           src/search/catalogsselector.cpp

HEADERS += src/data/storage.h \
           src/data/catalog.h \
	       src/data/bookitem.h \
           src/data/listitem.h \    
           src/data/textitem.h \       
           src/helpers/qregexphighlighter.h \
           src/search/searchwindow.h \
           src/search/booksselector.h \
           src/search/catalogsselector.h

FORMS   += src/search/searchwindow.ui \
           src/search/catalogsselector.ui \
           src/search/booksselector.ui 
           

RESOURCES += \
    bin/images.qrc \
    bin/basic.qrc \
    bin/tfs_rus.qrc \
    bin/tfs_quotes.qrc \
    bin/tfs_eng_part_1.qrc \
    bin/tfs_eng_part_2.qrc \
    bin/tfs_eng_part_3.qrc \
    bin/others.qrc \
    bin/spiritual_world.qrc \
    bin/tribal_messianship.qrc \
    bin/bible.qrc
