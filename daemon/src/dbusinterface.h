#ifndef DBusInterface_H
#define DBusInterface_H

#include <QObject>
#include <QtDBus/QtDBus>
#include <QStringList>
#include <mlite5/MGConfItem>
#include <mlite5/MNotification>
#include <mlite5/MDesktopEntry>

#define SERVICE_NAME "com.kimmoli.letohd"

class QDBusInterface;
class DBusInterface : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", SERVICE_NAME)

public:
    /* Pass application version as argument, no need for clean build */
    explicit DBusInterface(QString appVersion = "unknown", QObject *parent = 0);
    virtual ~DBusInterface();
    void registerDBus();

signals:
    void requestState(bool state);
    void requestLeds(QStringList leds);
    void testNotification(QString notification);

public slots:
    QString getVersion();
    void quit();
    void setLeds(QStringList leds) { emit requestLeds(leds); }
    void enable() { emit requestState(true); }
    void disable() { emit requestState(false); }
    void test(QString notification) { emit testNotification(notification); }

private:
    bool m_dbusRegistered;
    void showNotification(QString text);
    QString m_appVersion;

};

#endif // DBusInterface_H
