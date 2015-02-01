#include <stdio.h>
#include <QDBusConnection>
#include "callmonitor.h"

CallMonitor::CallMonitor(QObject *parent) :
    QObject(parent)
{
    m_isRinging = false;

    static QDBusConnection mceCallStateconn = QDBusConnection::systemBus();
    mceCallStateconn.connect("com.nokia.mce", "/com/nokia/mce/signal", "com.nokia.mce.signal", "sig_call_state_ind",
                          this, SLOT(handleCall(const QDBusMessage&)));

}

void CallMonitor::handleCall(const QDBusMessage& msg)
{
    QList<QVariant> args = msg.arguments();

    printf("sig_call_state_ind says: %s\n", qPrintable(args.at(0).toString()));

    if ( !(QString::localeAwareCompare( args.at(0).toString(), "ringing")) )
    {
        m_isRinging = true;
        emit incomingCall("ringing");
    }
    else if ( m_isRinging )
    {
        printf("Call answered, rejected or placing new call when missed call indicated\n");
        m_isRinging = false;
    }

}

void CallMonitor::notificationHandled(QString notification)
{
    if (m_isRinging && notification == "ringing")
        emit incomingCall("ringing");
}
