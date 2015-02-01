#ifndef CALLMONITOR_H
#define CALLMONITOR_H

#include <QObject>
#include <QDBusMessage>

class CallMonitor : public QObject
{
    Q_OBJECT
public:
    explicit CallMonitor(QObject *parent = 0);

signals:
    void incomingCall(QString notification);

public slots:
    void handleCall(const QDBusMessage& msg);
    void notificationHandled(QString notification);

private:
    bool m_isRinging;

};

#endif // CALLMONITOR_H
