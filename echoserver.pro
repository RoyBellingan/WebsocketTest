TARGET = echoserver
CONFIG   += console
CONFIG   -= app_bundle

QT = websockets

LIBS += -lpqxx
TEMPLATE = app

SOURCES += \
    appliance.cpp \
    config.cpp \
    main.cpp \
    echoserver.cpp

HEADERS += \
    appliance.h \
    config.h \
    echoserver.h

EXAMPLE_FILES += echoclient.html

include(rbk/rbk.pri)
