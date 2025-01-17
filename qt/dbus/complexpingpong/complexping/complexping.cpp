#include "../ping-common.h"
#include "complexping.h"

#include <stdio.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtDBus/QtDBus>

void Ping::start(const QString &name, const QString &oldValue, const QString &newValue)
{
    Q_UNUSED(oldValue);

    if (name != SERVICE_NAME || newValue.isEmpty()) {
        return;
    }

    qstdin.open(stdin, QIODevice::ReadOnly);

    iface = new QDBusInterface(SERVICE_NAME, "/", "com.redflag.QtDBus.ComplexPong.Pong", QDBusConnection::sessionBus(), this);

    if (!iface->isValid()) {
        fprintf(stderr, "%s\n", qPrintable(QDBusConnection::sessionBus().lastError().message()));

        QCoreApplication::instance()->quit();
    }

    connect(iface, SIGNAL(aboutToQuit()), QCoreApplication::instance(), SLOT(quit()));
    while (true) {
        printf("Ask your question: ");

        QString line = QString::fromLocal8Bit(qstdin.readLine()).trimmed();
        if (line.isEmpty()) {
            iface->call("quit");
            return;
        } else if (line == "value") {
            QVariant reply = iface->property("value");
            if (!reply.isNull()) {
                printf("value = %s\n", qPrintable(reply.toString()));
            }
        } else if (line.startsWith("value=")) {
            iface->setProperty("value", line.mid(6));
        } else {
            QDBusReply<QDBusVariant> reply = iface->call("query", line);
            if (reply.isValid()) {
                printf("Reply was:%s\n", qPrintable(reply.value().variant().toString()));
            }
        }

        if (iface->lastError().isValid()) {
            fprintf(stderr, "Call failed: %s\n", qPrintable(iface->lastError().message()));
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    if (!QDBusConnection::sessionBus().isConnected()) {
        fprintf(stderr, "Cann't connect to the D-Bus session bus.\n"
                        "To start it, run:\n"
                        "\teval `dbus-launch --auto-syntax`\n");
        return 1;
    }

    Ping ping;
    ping.connect(QDBusConnection::sessionBus().interface(), SIGNAL(serviceOwnerChanged(QString, QString, QString)), SLOT(start(QString, QString, QString)));

    QProcess pong;
    pong.start("../complexpong/complexpong");

    app.exec();
}
