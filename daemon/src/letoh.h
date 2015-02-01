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
    void notificationHandled(QString notification);

public slots:
    void handleNotify(QString notification);
    void handleNotificationClosed(const QDBusMessage &msg);
    void stateControl(bool state) { setLetohState(state); }
    QStringList setLeds(QStringList leds);

private:
    void initDriverData();
    void controlVdd(bool state);
    void setLetohState(bool active);

    QList<QStringList> loadLedSequence(QString notification);
    void showSequence(QList<QStringList> sequence);

    QVariantMap ledDrivers;

    PCA9685 *driver0;
    PCA9685 *driver1;
};

#endif // LETOH_H
