TARGET = echoserver
CONFIG   += console
CONFIG   -= app_bundle

QT = websockets

TEMPLATE = app

SOURCES += \
    appliance.cpp \
    main.cpp \
    echoserver.cpp

HEADERS += \
    appliance.h \
    echoserver.h

EXAMPLE_FILES += echoclient.html

include(rbk/rbk.pri)
