TARGET = echoserver
CONFIG   += console
CONFIG   -= app_bundle

#QMAKE_CXXFLAGS+= -fsanitize=address -fno-omit-frame-pointer
#QMAKE_CFLAGS+= -fsanitize=address -fno-omit-frame-pointer
#QMAKE_LFLAGS+= -fsanitize=address -fno-omit-frame-pointer

QT = websockets

#zypper in libpqxx-devel
LIBS += -lpqxx

TEMPLATE = app

SOURCES += \
    appliance.cpp \
    appliancemap.cpp \
    config.cpp \
    main.cpp \
    echoserver.cpp \
    pollcycle.cpp

HEADERS += \
    appliance.h \
    appliancemap.h \
    config.h \
    echoserver.h \
    pollcycle.h

EXAMPLE_FILES += echoclient.html

include(rbk/rbk.pri)
