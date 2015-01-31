#ifndef LETOH_H
#define LETOH_H

#include <QObject>
#include <QDBusMessage>

class Letoh : public QObject
{
    Q_OBJECT
public:
    explicit Letoh(QObject *parent = 0);

signals:

public slots:
    void handleNotify(QString notification);
    void handleNotificationClosed(const QDBusMessage &msg);

};

#endif // LETOH_H
