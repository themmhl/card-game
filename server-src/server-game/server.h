#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QList>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include "useraccount.h"

enum SERVER_CODES { LOGIN, SIGNUP, LOGOUT, EDIT_INFO, FORGOT_PASSWORD, START_GAME };
class Server:public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent = nullptr);
    ~Server();
    void startServer(int port);
signals:
     void gameReadyToStart();
private slots:
    void handleNewConnection();
    void handleDisconnected();
    void processClientRequest();
    void triggerNewGame();

private:
    void handleStartGame(const QJsonObject& request, QTcpSocket* socket);
    void handleSignUp(const QJsonObject& request, QTcpSocket* socket);
    void handleLogin(const QJsonObject& request, QTcpSocket* socket);
    void handleLogOut(const QJsonObject& request, QTcpSocket* socket);
    void handleEditInfo(const QJsonObject& request, QTcpSocket* socket);
    void handleForgetPassword(const QJsonObject& request, QTcpSocket* socket);
    void sendJsonReact(QTcpSocket* socket, SERVER_CODES request,QString message);
    void addNewSocket(QTcpSocket *socket);
    QTcpServer *tcpServer;
    QList<QTcpSocket *>socketList;
    QMap<QString,UserAccount> dataBase;
    QMap<QTcpSocket*, QString> loggedInUsers;
    QList<QTcpSocket*> waitingQueue;

};


#endif // SERVER_H
