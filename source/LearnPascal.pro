#-------------------------------------------------
#
# Project created by QtCreator 2017-12-22T19:02:16
#
#-------------------------------------------------

QT       += core gui sql widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

VERSION = 1.0
QMAKE_TARGET_COMPANY = Kiselev Corp
QMAKE_TARGET_PRODUCT = Learn Pascal
QMAKE_TARGET_DESCRIPTION = Application for learning Pascal
QMAKE_TARGET_COPYRIGHT = Alex Kiselev

TARGET = LearnPascal
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    logindialog.cpp \
    profileform.cpp \
    openprofiledialog.cpp \
    createprofiledialog.cpp \
    setnewpassdialog.cpp \
    tasksdialog.cpp \
    createtaskdialog.cpp \
    edittaskdialog.cpp \
    aboutdialog.cpp

HEADERS += \
        mainwindow.h \
    header.h \
    logindialog.h \
    profileform.h \
    openprofiledialog.h \
    createprofiledialog.h \
    setnewpassdialog.h \
    tasksdialog.h \
    createtaskdialog.h \
    edittaskdialog.h \
    aboutdialog.h

FORMS += \
        mainwindow.ui \
    logindialog.ui \
    profileform.ui \
    openprofiledialog.ui \
    createprofiledialog.ui \
    setnewpassdialog.ui \
    tasksdialog.ui \
    createtaskdialog.ui \
    edittaskdialog.ui \
    aboutdialog.ui

DISTFILES += \
    icon.ico

RC_ICONS = icon.ico

RESOURCES += \
    res.qrc
