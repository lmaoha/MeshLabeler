QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    UI/colortablewidget.cpp \
    UI/mytablewidget.cpp \
    VTK/designinteractorstyle.cpp \
    VTK/vtkshow.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    UI/colortablewidget.h \
    UI/mytablewidget.h \
    VTK/designinteractorstyle.h \
    VTK/vtkshow.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#在Release 下也可以debug
QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
QMAKE_LFLAGS_RELEASE = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO

UI_DIR = $$PWD
msvc {
QMAKE_CFLAGS += /utf-8
QMAKE_CXXFLAGS += /utf-8
}
include(VTKlib.pri)


