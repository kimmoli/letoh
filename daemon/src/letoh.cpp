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
