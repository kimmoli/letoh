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

    setLetohState(true);

    showSequence( loadLedSequence(notification) );

    setLetohState(false);
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
    controlVdd(active);

    QThread::msleep(100);

    if (active)
    {
        driver1 = new PCA9685(0x41);
        driver0 = new PCA9685(0x40);
    }
}

QStringList Letoh::setLeds(QStringList leds)
{
    QStringList tmp;

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
        printf("%s ", qPrintable(leds.at(i)));
        tmp << leds.at(i);
        i = (i + 1) % leds.count();
    }

    printf("\n");

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

    return tmp;
}

QList<QStringList> Letoh::loadLedSequence(QString notification)
{
    QFile file("/usr/share/harbour-letoh/ledsequences");
    QList<QStringList> tmp;

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        printf("failed to open\n");
        return tmp;
    }

    QTextStream in(&file);
    bool found = false;

    while (!in.atEnd())
    {

        QString line = in.readLine();

        if (line.at(0) == QChar('['))
        {
            found = line.contains(notification, Qt::CaseInsensitive);
        }
        else if (found)
        {
            tmp.append(line.split(QChar(' ')));
        }
    }
    file.close();

    return tmp;
}

void Letoh::showSequence(QList<QStringList> sequence)
{
    QStringList prevLeds = setLeds(QStringList() << "#000000");

    foreach (QStringList step, sequence)
    {
        /* letoh scripting:
         *
         * row starts always with command
         * colors are repeated until all leds have color
         * single given color will be set to all leds
         * two given colors will be set repeatedly to all leds
         *
         * delay [time in ms]
         * set [color,..]
         * fade [fading duration in ms] [color,..]
         * shift [ccw|cw]
         */

        if (step.at(0).startsWith("delay", Qt::CaseInsensitive))
        {
            QThread::msleep(step.at(1).toInt());
        }
        else if (step.at(0).startsWith("set", Qt::CaseInsensitive))
        {
            step.removeFirst();
            prevLeds = setLeds(step);
        }
        else if (step.at(0).startsWith("shift", Qt::CaseInsensitive))
        {
            if (step.at(1).startsWith("cw", Qt::CaseInsensitive))
                prevLeds << prevLeds.takeFirst();
            else
                prevLeds.insert(0, prevLeds.takeLast());
            setLeds(prevLeds);
        }
        else if (step.at(0).startsWith("fade", Qt::CaseInsensitive))
        {
            /* one fade step is 50 ms */
            int duration = step.at(1).toInt();

            step.removeFirst();
            step.removeFirst();

            /* expand color array */
            QStringList target;
            int n=0;
            for (int i=0 ; i<10 ; i++)
            {
                target << step.at(n);
                n = (n + 1) % step.count();
            }

            /*  */
            int steps = duration/50;

            printf("there are %d steps in %d\n", steps, duration);

            for (int i=0 ; i<steps ; i++)
            {
                QThread::msleep(50);
                QStringList thisStep;

                for (int j=0 ; j<10 ; j++)
                {
                    int r0 = QColor(prevLeds.at(j)).red();
                    int g0 = QColor(prevLeds.at(j)).green();
                    int b0 = QColor(prevLeds.at(j)).blue();
                    int r2 = QColor(target.at(j)).red();
                    int g2 = QColor(target.at(j)).green();
                    int b2 = QColor(target.at(j)).blue();
                    int r1 = ((r2-r0)/steps)*i + r0;
                    int g1 = ((g2-g0)/steps)*i + g0;
                    int b1 = ((b2-b0)/steps)*i + b0;
                    QColor thisColor(r1, g1, b1);
                    thisStep << thisColor.name();
                }
                setLeds(thisStep);
            }
            prevLeds = setLeds(target);
        }
    }
}
