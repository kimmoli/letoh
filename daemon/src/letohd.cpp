#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>

#include <QtGlobal>
#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtDBus/QtDBus>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QSignalMapper>

#include "letohd.h"
#include "notificationmanager.h"
#include "adaptor.h"
#include "dbusinterface.h"
#include "letoh.h"

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    daemonize();

    setlinebuf(stdout);
    setlinebuf(stderr);

    printf("Starting letohd daemon. Version %s.\n", APPVERSION);

    DBusInterface dbif(QString(APPVERSION));
    new LetohdAdaptor(&dbif);

    dbif.registerDBus();

    Letoh letoh;

    NotificationManager notifications;

    notifications.connect(&notifications, SIGNAL(notify(QString)), &letoh, SLOT(handleNotify(QString)));

    QDBusConnection::sessionBus().connect("org.freedesktop.Notifications",
                                          "/org/freedesktop/Notifications",
                                          "org.freedesktop.Notifications",
                                          "NotificationClosed",
                                          &letoh, SLOT(handleNotificationClosed(const QDBusMessage&)));

    return app.exec();

}


void daemonize()
{
	/* Change the file mode mask */
	umask(0);

	/* Change the current working directory */
	if ((chdir("/tmp")) < 0) 
		exit(EXIT_FAILURE);

	/* register signals to monitor / ignore */
	signal(SIGCHLD,SIG_IGN); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,signalHandler); /* catch hangup signal */
	signal(SIGTERM,signalHandler); /* catch kill signal */
    signal(SIGINT,signalHandler);

    qInstallMessageHandler(myMessageOutput);
}


void signalHandler(int sig) /* signal handler function */
{
	switch(sig)
	{
		case SIGHUP:
            printf("Received signal SIGHUP\n");
			break;		
		case SIGTERM:
            printf("Received signal SIGTERM\n");
/* Todo replace with letoh stuff */
//            deinitOled();
//            controlVdd(0);
			exit(0);
			break;		
        case SIGINT:
            printf("Received signal SIGINT\n");
/* Todo replace with letoh stuff */
//            deinitOled();
//            controlVdd(0);
            exit(0);
            break;
    }
}


void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    }
}
