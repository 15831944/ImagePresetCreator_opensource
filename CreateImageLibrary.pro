#-------------------------------------------------
#
# Project created by QtCreator 2017-12-24T17:06:23
#
#-------------------------------------------------

QT       += core gui    \
            svg         \
            xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LibraryCreator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \
           APP_TARGET=\\\"$$TARGET\\\"
TRANSLATIONS += translate/\"$$TARGET\"_ja.ts \
                translate/\"$$TARGET\"_en.ts
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES +=  src/main.cpp                         \
            src/ui/dialog/MainWindow.cpp         \
            src/listView/listedDataCore.cpp      \
            src/listView/libraryListedData.cpp   \
            src/transfer/svgColor.cpp            \
            src/transfer/svgParser.cpp           \
            src/transfer/svgPath.cpp             \
            src/transfer/svgPathEle.cpp          \
            src/transfer/svgTransform.cpp        \
            src/transfer/unitTransfer.cpp        \
            src/transfer/kdTree.cpp              \
            src/transfer/dxfParser.cpp           \
            src/transfer/dxfDefinedColor.cpp \
            src/ui/widget/customPushButton.cpp

HEADERS  += src/common/common.h                  \
            src/ui/dialog/MainWindow.h           \
            src/listView/listedDataCore.h        \
            src/listView/libraryListedData.h     \
            src/transfer/parserCore.h            \
            src/transfer/svgColor.h              \
            src/transfer/svgCommon.h             \
            src/transfer/svgParser.h             \
            src/transfer/svgPath.h               \
            src/transfer/svgPathEle.h            \
            src/transfer/svgTransform.h          \
            src/transfer/unitTransfer.h          \
            src/transfer/kdTree.cpp              \
            src/transfer/dxfParser.h             \
            src/transfer/dxfDefinedColor.h \
            src/ui/widget/customPushButton.h

INCLUDEPATH += src \
        src/ui/dialog \
        src/ui/widget

FORMS    += src/ui/dialog/MainWindow.ui

RC_ICONS = app.ico

DISTFILES += fabool_fomat.astylerc

RESOURCES += \
    rc.qrc
