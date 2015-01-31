#include <stdio.h>

#include "letoh.h"

Letoh::Letoh(QObject *parent) :
    QObject(parent)
{
}

void Letoh::handleNotify(QString notification)
{
    printf("Notification: %s\n", qPrintable(notification));
}

void Letoh::handleNotificationClosed(const QDBusMessage &msg)
{
    Q_UNUSED(msg);
    printf("Notification closed.\n");
}
