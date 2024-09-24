QT       += core gui network multimedia sql

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
    customdelegete.cpp \
    globalplayer.cpp \
    main.cpp \
    mainwindow.cpp \
    mylove.cpp \
    recent_play.cpp \
    search.cpp \
    select.cpp \
    sqltext.cpp \
    zhuche.cpp

HEADERS += \
    comment.h \
    customdelegete.h \
    globalplayer.h \
    mainwindow.h \
    mylove.h \
    recent_play.h \
    search.h \
    select.h \
    sqltext.h \
    zhuche.h

FORMS += \
    form.ui \
    mainwindow.ui \
    mylove.ui \
    recent_play.ui \
    search.ui \
    select.ui \
    zhuche.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    picture.qrc

DISTFILES +=
