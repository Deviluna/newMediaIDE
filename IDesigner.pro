#-------------------------------------------------
#
# Project created by QtCreator 2017-02-17T16:29:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IDesigner
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    addfiledialog.cpp \
    createprojectdialog.cpp \
    adddirdialog.cpp \
    testwidget.cpp \
    closedialog.cpp \
    welcomedialog.cpp \
    rmdirdialog.cpp \
    argall.cpp \
    generatedialog.cpp \
    introdialog.cpp \
    renamedialog.cpp \
    dirprodialog.cpp \
    seturldialog.cpp

HEADERS  += mainwindow.h \
    addfiledialog.h \
    createprojectdialog.h \
    adddirdialog.h \
    testwidget.h \
    closedialog.h \
    welcomedialog.h \
    rmdirdialog.h \
    argall.h \
    generatedialog.h \
    introdialog.h \
    renamedialog.h \
    dirprodialog.h \
    seturldialog.h

FORMS    += mainwindow.ui \
    addfiledialog.ui \
    createprojectdialog.ui \
    adddirdialog.ui \
    testwidget.ui \
    closedialog.ui \
    welcomedialog.ui \
    rmdirdialog.ui \
    generatedialog.ui \
    introdialog.ui \
    renamedialog.ui \
    dirprodialog.ui \
    seturldialog.ui

RESOURCES += \
    designer.qrc

RC_FILE =qtdemo.rc
