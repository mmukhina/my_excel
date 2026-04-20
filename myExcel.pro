QT       += core gui qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myExcel
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    delegate.cpp

HEADERS += \
    mainwindow.h \
    delegate.h

FORMS += \
    mainwindow.ui
