TARGET = harbour-letohd

CONFIG += console link_pkgconfig
PKGCONFIG += contextkit-statefs mlite5

QT += dbus

LIBS += -lrt

# DBus
system(qdbusxml2cpp config/com.kimmoli.letohd.xml -i src/dbusinterface.h -a src/adaptor)

target.path = /usr/bin/

systemd.path = /etc/systemd/system/
systemd.files = config/$${TARGET}.service

udevrule.path = /etc/udev/rules.d/
udevrule.files = config/95-$${TARGET}.rules

dbusconf.path = /etc/dbus-1/system.d/
dbusconf.files = config/$${TARGET}.conf

dbusInterface.files = config/com.kimmoli.letohd.xml
dbusInterface.path = /usr/share/dbus-1/interfaces/

DEFINES += "APPVERSION=\\\"$${SPECVERSION}\\\""

message($${DEFINES})

INSTALLS += target systemd udevrule dbusconf dbusInterface

INCLUDEPATH += \
    ./3rdparty/libdsme/include/ \
    ./3rdparty/mce-dev/include/ \
    ./3rdparty/libiphb/src/


SOURCES += \
    3rdparty/libiphb/src/libiphb.c \
    src/letohd.cpp \
    src/dbusinterface.cpp \
    src/adaptor.cpp \
    src/notificationmanager.cpp \
    src/letoh.cpp

HEADERS += \
    3rdparty/libiphb/src/libiphb.h \
    3rdparty/libiphb/src/iphb_internal.h \
    3rdparty/libdsme/include/dsme/messages.h \
    3rdparty/mce-dev/include/mce/dbus-names.h \
    src/letohd.h \
    src/dbusinterface.h \
    src/adaptor.h \
    src/notificationmanager.h \
    src/letoh.h

OTHER_FILES += \
    config/$${TARGET}.conf \
    config/$${TARGET}.service \
    config/95-$${TARGET}.rules \
    config/com.kimmoli.letohd.xml

