#include <stdio.h>

#include <QThread>
#include <QVariantMap>
#include <QFile>
#include <QTextStream>
#include <QColor>

#include "letoh.h"

Letoh::Letoh(QObject *parent) :
    QObject(parent)
{
    initDriverData();
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

void Letoh::initDriverData()
{
    QVariantMap driver;

    /* Assign I2C address, PCA9685 ports and default colors */

    driver["address"] = 0x40; driver["red"] = 1; driver["green"] = 0; driver["blue"] = 2; driver["color"] = QColor("#000000");
    ledDrivers["topright"] = QVariant(driver);
    driver["address"] = 0x40; driver["red"] = 4; driver["green"] = 3; driver["blue"] = 5; driver["color"] = QColor("#000000");
    ledDrivers["upperright"] = QVariant(driver);
    driver["address"] = 0x40; driver["red"] = 7; driver["green"] = 6; driver["blue"] = 8; driver["color"] = QColor("#000000");
    ledDrivers["middleright"] = QVariant(driver);
    driver["address"] = 0x40; driver["red"] = 10; driver["green"] = 9; driver["blue"] = 11; driver["color"] = QColor("#000000");
    ledDrivers["lowerright"] = QVariant(driver);
    driver["address"] = 0x40; driver["red"] = 13; driver["green"] = 12; driver["blue"] = 14; driver["color"] = QColor("#000000");
    ledDrivers["bottomright"] = QVariant(driver);

    driver["address"] = 0x41; driver["red"] = 1; driver["green"] = 0; driver["blue"] = 2; driver["color"] = QColor("#000000");
    ledDrivers["bottomleft"] = QVariant(driver);
    driver["address"] = 0x41; driver["red"] = 4; driver["green"] = 3; driver["blue"] = 5; driver["color"] = QColor("#000000");
    ledDrivers["lowerleft"] = QVariant(driver);
    driver["address"] = 0x41; driver["red"] = 7; driver["green"] = 6; driver["blue"] = 8; driver["color"] = QColor("#000000");
    ledDrivers["middleleft"] = QVariant(driver);
    driver["address"] = 0x41; driver["red"] = 10; driver["green"] = 9; driver["blue"] = 11; driver["color"] = QColor("#000000");
    ledDrivers["upperleft"] = QVariant(driver);
    driver["address"] = 0x41; driver["red"] = 13; driver["green"] = 12; driver["blue"] = 14; driver["color"] = QColor("#000000");
    ledDrivers["topleft"] = QVariant(driver);

#if 0
    for(QVariantMap::const_iterator iter = ledDrivers.begin(); iter != ledDrivers.end(); ++iter)
    {
        qDebug() << iter.key() << " = " << qvariant_cast<QVariantMap>(iter.value())["color"].toString();
    }
#endif
}

void Letoh::controlVdd(bool state)
{
    QFile outputFile("/sys/devices/platform/reg-userspace-consumer.0/state");

    if (outputFile.open(QFile::WriteOnly | QFile::Truncate))
    {
        QTextStream out(&outputFile);
        out << (state ? "1" : "0");
        outputFile.close();
    }

    printf("power %s\n", state ? "on" : "off");
}

void Letoh::setLetohState(bool active)
{
    if (active == getLetohState())
    {
        printf("WTF, state already %s\n", active ? "active" : "inactive");
        return;
    }

    controlVdd(active);

    QThread::msleep(100);

    if (active)
    {
        driver1 = new PCA9685(0x41);
        driver0 = new PCA9685(0x40);
    }
}

bool Letoh::getLetohState()
{
    QString line;

    QFile inputFile( "/sys/devices/platform/reg-userspace-consumer.0/state" );

    if ( inputFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
       QTextStream in( &inputFile );
       line = in.readLine();
       inputFile.close();
    }

    return line.at(0) == QChar('1');
}

void Letoh::setLeds(QStringList leds)
{

    QStringList ledNames;
    ledNames << "topleft";
    ledNames << "upperleft";
    ledNames << "middleleft";
    ledNames << "lowerleft";
    ledNames << "bottomleft";
    ledNames << "bottomright";
    ledNames << "lowerright";
    ledNames << "middleright";
    ledNames << "upperright";
    ledNames << "topright";

    int i = 0;

    foreach (QString ledName, ledNames)
    {
        QVariantMap t = qvariant_cast<QVariantMap>(ledDrivers[ledName]);
        t["color"] = QColor(leds.at(i));
        ledDrivers[ledName] = QVariant(t);
        i = (i + 1) % leds.count();
    }

#if 0
    char data[120] = { 0 };

    for(QVariantMap::const_iterator iter = ledDrivers.begin(); iter != ledDrivers.end(); ++iter)
    {
        int offset = 0;

        QVariantMap t = qvariant_cast<QVariantMap>(ledDrivers[iter.key()]);

        if (t["address"].toInt() == 0x41)
            offset = 60;

        int red = QColor(t["color"].toString()).red();
        int green = QColor(t["color"].toString()).green();
        int blue = QColor(t["color"].toString()).blue();

        /* Setting maximun dutycycle to 50% for safety during testing */
        int ledOnRed = 2047;
        int ledOnGreen = 1023;
        int ledOnBlue = 0;

        int ledOffRed = ledOnRed + 8*red;
        int ledOffGreen = ledOnGreen + 12*green;
        int ledOffBlue = ledOnBlue + 8*blue;

        data[offset + 4*t["red"].toInt()+0] = ledOnRed & 0xff;
        data[offset + 4*t["red"].toInt()+1] = (ledOnRed >> 8) & 0xff;
        data[offset + 4*t["red"].toInt()+2] = ledOffRed & 0xff;
        data[offset + 4*t["red"].toInt()+3] = (ledOffRed >> 8) & 0xff;
        data[offset + 4*t["green"].toInt()+0] = ledOnGreen & 0xff;
        data[offset + 4*t["green"].toInt()+1] = (ledOnGreen >> 8) & 0xff;
        data[offset + 4*t["green"].toInt()+2] = ledOffGreen & 0xff;
        data[offset + 4*t["green"].toInt()+3] = (ledOffGreen >> 8) & 0xff;
        data[offset + 4*t["blue"].toInt()+0] = ledOnBlue & 0xff;
        data[offset + 4*t["blue"].toInt()+1] = (ledOnBlue >> 8) & 0xff;
        data[offset + 4*t["blue"].toInt()+2] = ledOffBlue & 0xff;
        data[offset + 4*t["blue"].toInt()+3] = (ledOffBlue >> 8) & 0xff;

        //qDebug() << iter.key() << " = " << qvariant_cast<QVariantMap>(iter.value())["color"].toString();
    }

    driver0->updateLeds(data,0,60);
    driver1->updateLeds(data,60,60);
#endif
}
