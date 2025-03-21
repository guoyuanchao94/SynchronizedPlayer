QT       += core gui multimedia multimediawidgets network quick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000
# disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    client.cpp \
    connecttoserver.cpp \
    main.cpp \
    mainwindow.cpp \
    openurl.cpp \
    server.cpp \
    setport.cpp \
    udpclient.cpp \
    udpconnection.cpp \
    udpserver.cpp

HEADERS += \
    client.h \
    connecttoserver.h \
    mainwindow.h \
    openurl.h \
    server.h \
    setport.h \
    udpclient.h \
    udpconnection.h \
    udpserver.h

FORMS += \
    connecttoserver.ui \
    mainwindow.ui \
    openurl.ui \
    setport.ui
RC_ICONS += logo.ico
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    themes.qrc
