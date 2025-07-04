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
#include "player.h"
#include "card.h"
#include "game.h"
#include "handlecard.h"
#include "sharedDefs.h"

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
    void onGameOver();

private:
    void handleStartGame(const QJsonObject& request, QTcpSocket* socket);
    void handleSignUp(const QJsonObject& request, QTcpSocket* socket);
    void handleLogin(const QJsonObject& request, QTcpSocket* socket);
    void handleLogOut(const QJsonObject& request, QTcpSocket* socket);
    void handleEditInfo(const QJsonObject& request, QTcpSocket* socket);
    void handleForgetPassword(const QJsonObject& request, QTcpSocket* socket);
    void handleGetAvailableCards(const QJsonObject& request, QTcpSocket* socket);
    void handleGetPlayers(const QJsonObject& request, QTcpSocket* socket);
    void sendJsonReact(QTcpSocket* socket, SERVER_CODES request,QString message);
    void addNewSocket(QTcpSocket *socket);
    QTcpServer *tcpServer;
    QList<QTcpSocket *>socketList;
    QMap<QString,UserAccount> dataBase;
    QMap<QTcpSocket*, QString> loggedInUsers;
    QList<QTcpSocket*> waitingQueue;
    Game* game;
};


#endif // SERVER_H
