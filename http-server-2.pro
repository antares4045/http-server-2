QT -= gui widgets
QT += core network concurrent sql

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        sources/HttpServer.cpp \
        main.cpp \
        sources/TaskManager.cpp \
        sources/TestQObjects.cpp \
        sources/TestServer.cpp \
        sources/ThreadManager.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    includes/DBConnecionFactory.h \
    includes/TaskManager.h \
    test/TestQObjects.h \
    test/TestServer.h \
    utils/HttpServer.h \
    includes/ThreadManager.h

RESOURCES = \
    interface.qrc
