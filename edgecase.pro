# Qt libraries config
QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# app settings
TARGET   = edgecase
TEMPLATE = app

# source code
SOURCES += src/googletestprogram.cpp \
           src/main.cpp \
           src/mainwindow.cpp \
           src/programfinder.cpp \
           src/programtypeselector.cpp \
           src/qtestlibprogram.cpp \
           src/resultdetailsview.cpp \
           src/testcase.cpp \
           src/testprogram.cpp \
           src/testprogramfactory.cpp \
           src/testprogressbar.cpp \
           src/testrunner.cpp \
           src/testsuite.cpp \
           src/testlistview.cpp

HEADERS += src/googletestprogram.h \
           src/mainwindow.h \
           src/programfinder.h \
           src/programinfo.h \
           src/programtypeselector.h \
           src/qtestlibprogram.h \
           src/resultdetailsview.h \
           src/testcase.h \
           src/testprogram.h \
           src/testprogramfactory.h \
           src/testprogressbar.h \
           src/testrunner.h \
           src/testsuite.h \
           src/testlistview.h

RESOURCES += edgecase.qrc

OTHER_FILES += LICENSE \
               README
