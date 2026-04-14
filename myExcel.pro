QT       += core gui qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = myExcel
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    delegate.cpp          # MAKE SURE THIS LINE EXISTS!

HEADERS += \
    mainwindow.h \
    delegate.h            # MAKE SURE THIS LINE EXISTS!

FORMS += \
    mainwindow.ui
