QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    KG.cpp \
    graph.cpp \
    main.cpp \
    mainwindow.cpp \
    point.cpp \
    tdetail.cpp

HEADERS += \
    KG.h \
    graph.h \
    linklist.h \
    mainwindow.h \
    point.h \
    tdetail.h

FORMS += \
    mainwindow.ui \
    tdetail.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Style.qrc
