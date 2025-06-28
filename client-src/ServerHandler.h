#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H
#include <QTcpSocket>
#include <QObject>
#include <QtQml/qqmlregistration.h>
#include "Player.h"
using namespace std;

enum SERVER_CODES { LOGIN, SIGNUP, LOGOUT, EDIT_INFO, FORGOT_PASSWORD };

class ServerHandler : public QObject
{
    Q_OBJECT
    QString ip_address;
    int port;
    QTcpSocket* socket;
    Player* player;
    bool read_done;

public:
    QString server_messages[10];

    QString get_ip_address() const;
    void set_ip_address(const QString& ip_address);
    int get_port() const;
    void set_port(int port);

    explicit ServerHandler(QObject* parent);
    bool connect_to_server();
    bool sign_up(QString, QString, QString, QString, QString, QString);
    bool login(QString username, QString password_raw);
    bool logout();
    bool forgot_password(QString, QString);
    bool edit_info(QString, QString, QString, QString, QString, QString);
    bool read_all(SERVER_CODES type);

private slots:
    void readyRead();
    void error();
};


#endif //SERVERHANDLER_H
