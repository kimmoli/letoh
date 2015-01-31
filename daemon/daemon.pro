TARGET = harbour-letohd

CONFIG += console link_pkgconfig
PKGCONFIG += contextkit-statefs

QT += dbus

LIBS += -lrt

target.path = /usr/bin/

systemd.path = /etc/systemd/system/
systemd.files = config/$${TARGET}.service

udevrule.path = /etc/udev/rules.d/
udevrule.files = config/95-$${TARGET}.rules

dbusconf.path = /etc/dbus-1/system.d/
dbusconf.files = config/$${TARGET}.conf

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

message($${DEFINES})

INSTALLS += target systemd udevrule dbusconf

INCLUDEPATH += \
    ./3rdparty/libdsme/include/ \
    ./3rdparty/mce-dev/include/ \
    ./3rdparty/libiphb/src/


SOURCES += \
    3rdparty/libiphb/src/libiphb.c \
    src/letohd.cpp \
    src/notificationmanager.cpp

HEADERS += \
    3rdparty/libiphb/src/libiphb.h \
    3rdparty/libiphb/src/iphb_internal.h \
    3rdparty/libdsme/include/dsme/messages.h \
    3rdparty/mce-dev/include/mce/dbus-names.h \
    src/letohd.h \
    src/notificationmanager.h

OTHER_FILES += \
    config/$${TARGET}.conf \
    config/$${TARGET}.service \
    config/95-$${TARGET}.rules

