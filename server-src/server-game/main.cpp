#include <QCoreApplication>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QDebug>
#include "server.h"
QString getWifiIPv4Address() {
    const QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for (const QHostAddress &address : addresses) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol &&
            !address.isLoopback()) {
            return address.toString();
            }
    }
    return "127.0.0.1";
}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("Card Game Server");

    Server myServer;
    quint16 port = 8080;

    myServer.startServer(port);


    QString ipAddress = getWifiIPv4Address();
    qDebug() << "Server started.";
    qDebug() << "Connect clients to IP:" << ipAddress << "on Port:" << port;

    return a.exec();
}
