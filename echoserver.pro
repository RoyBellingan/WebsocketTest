TARGET = echoserver
CONFIG   += console
CONFIG   -= app_bundle

QT = websockets

#zypper in libpqxx-devel
LIBS += -lpqxx

TEMPLATE = app

SOURCES += \
    appliance.cpp \
    appliancemap.cpp \
    config.cpp \
    main.cpp \
    echoserver.cpp

HEADERS += \
    appliance.h \
    appliancemap.h \
    config.h \
    echoserver.h

EXAMPLE_FILES += echoclient.html

include(rbk/rbk.pri)
