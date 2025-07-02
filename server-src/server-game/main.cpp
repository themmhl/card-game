#include <QCoreApplication>
#include <QNetworkInterface>
#include <QHostInfo>
#include <QDebug>
#include "server.h"
QString getWifiIPv4Address() {
    QString wifiIPv4;
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    foreach (const QNetworkInterface& iface, interfaces) {
        if (iface.flags().testFlag(QNetworkInterface::IsUp) &&
            iface.flags().testFlag(QNetworkInterface::IsRunning) &&
            iface.flags().testFlag(QNetworkInterface::IsLoopBack) == false) {

            QList<QNetworkAddressEntry> entries = iface.addressEntries();
            foreach (const QNetworkAddressEntry& entry, entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    wifiIPv4 = entry.ip().toString();
                    if (iface.humanReadableName().contains("wlan", Qt::CaseInsensitive) ||
                        iface.humanReadableName().contains("wi-fi", Qt::CaseInsensitive) ||
                        iface.humanReadableName().contains("wireless", Qt::CaseInsensitive)) {
                        return wifiIPv4;
                        }
                }
            }
            }
    }
    return QString();
}



int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    a.setApplicationName("Card Game Server");

    Server myServer;
    quint16 port = 1234;

    myServer.startServer(port);


    QString ipAddress = getWifiIPv4Address();
    qDebug() << "Server started.";
    qDebug() << "Connect clients to IP:" << ipAddress << "on Port:" << port;

    return a.exec();
}
