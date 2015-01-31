#ifndef LETOH_H
#define LETOH_H

#include <QObject>
#include <QDBusMessage>
#include "pca9685.h"

class Letoh : public QObject
{
    Q_OBJECT
public:
    explicit Letoh(QObject *parent = 0);

signals:

public slots:
    void handleNotify(QString notification);
    void handleNotificationClosed(const QDBusMessage &msg);
    void powerControl(bool state) { controlVdd(state); }
    void setLeds(QStringList leds);

private:
    void initDriverData();
    void controlVdd(bool state);
    void setLetohState(bool active);
    bool getLetohState();

    QVariantMap ledDrivers;

    PCA9685 *driver0;
    PCA9685 *driver1;

};

#endif // LETOH_H
