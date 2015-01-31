#include <stdio.h>
#include "dbusinterface.h"

static const char *SERVICE = SERVICE_NAME;
static const char *PATH = "/";

DBusInterface::DBusInterface(QString appVersion, QObject *parent) :
    QObject(parent)
{
    m_dbusRegistered = false;
    m_appVersion = appVersion;
}

DBusInterface::~DBusInterface()
{
    if (m_dbusRegistered)
    {
        QDBusConnection connection = QDBusConnection::sessionBus();
        connection.unregisterObject(PATH);
        connection.unregisterService(SERVICE);

        printf("Unregistered from dbus sessionBus\n");
    }
}

void DBusInterface::registerDBus()
{
    if (!m_dbusRegistered)
    {
        // DBus
        QDBusConnection connection = QDBusConnection::systemBus();
        if (!connection.registerService(SERVICE))
        {
            QCoreApplication::quit();
            return;
        }

        if (!connection.registerObject(PATH, this))
        {
            QCoreApplication::quit();
            return;
        }
        m_dbusRegistered = true;

        printf("Succesfully registered to dbus systemBus \"%s\"\n", SERVICE);

        showNotification("LeTOH daemon started");
    }
}

void DBusInterface::quit()
{
    printf("Quit requested from dbus\n");
    QCoreApplication::quit();
}

QString DBusInterface::getVersion()
{
    return m_appVersion;
}

/******** PRIV *********/

/* show notification
 */
void DBusInterface::showNotification(QString text)
{
    MNotification notification(MNotification::DeviceEvent, "", text);
    notification.setImage(""); /* Todo own icon for LeTOH, if it needs notifications */
    notification.publish();
}
