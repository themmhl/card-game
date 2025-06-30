#include "server.h"
#include <QDebug>




Server::Server(QObject *parent): QObject(parent)
{
    tcpServer = new QTcpServer(this);
    connect(tcpServer, &QTcpServer::newConnection,this,&Server::handleNewConnection);
    connect(this, &Server::gameReadyToStart, this, &Server::triggerNewGame);
    game = nullptr;

}

Server::~Server() {}

void Server::startServer(int port) {
    if (tcpServer->listen(QHostAddress::Any, port)) {
        qDebug() << "Server started/ listening on port" << port;
    } else {
        qDebug() << "Server failed to start:" << tcpServer->errorString();
    }
}

void Server::handleNewConnection()
{
    while(tcpServer->hasPendingConnections()){
        if (socketList.size() >= 4) {
            qDebug() << "Server is full .";
            QTcpSocket *extraSocket = tcpServer->nextPendingConnection();
            extraSocket->close();
            extraSocket->deleteLater();
        }
        addNewSocket(tcpServer->nextPendingConnection());

    }

}

void Server::handleDisconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if(!socket){
        return;
    }
    if (this->loggedInUsers.contains(socket)){
        QString username = this->loggedInUsers.value(socket);
        this->loggedInUsers.remove(socket);
        qDebug()<<"session socket with username:"<<username<<"closed";
    }

    this->socketList.removeOne(socket);
    socket->deleteLater();
    qDebug() << "Client disconnected. Total clients:" << this->socketList.size();

}

void Server::addNewSocket(QTcpSocket *socket)
{
    socketList.append(socket);
    connect(socket, &QTcpSocket::readyRead, this, &Server::processClientRequest);
    connect(socket, &QTcpSocket::disconnected, this, &Server::handleDisconnected);
    qDebug() <<"new client with seocketDescriptor:"<<QString::number(socket->socketDescriptor())<< "connected";

}

void Server::processClientRequest()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    if(!socket) return;

    QByteArray jsonData = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);

    if (doc.isNull() || !doc.isObject()) {
        qDebug() << "receiv invalid json from client.";
        return;
    }

    QJsonObject requestObject = doc.object();
    if (!requestObject.contains("type")) {
        qDebug() << "json from client is missing 'type' field.";
        return;
    }

    SERVER_CODES type = static_cast<SERVER_CODES>(requestObject["type"].toInt());

    switch (type) {
    case SIGNUP:
        handleSignUp(requestObject, socket);
        break;
    case LOGIN:
        handleLogin(requestObject, socket);
        break;
    case LOGOUT:
        handleLogOut(requestObject,socket);
        break;
    case EDIT_INFO:
        handleEditInfo(requestObject,socket);
        break;
    case FORGOT_PASSWORD:
        handleForgetPassword(requestObject,socket);
        break;
    case START_GAME:
        handleStartGame(requestObject,socket);
        break;
    case GET_GAME_STATE:
        break;
    case GET_AVAILABLE_CARDS:
        handleGetAvailableCards(requestObject,socket);
        break;
    case CHOOSE_CARD:
        if (this->game != nullptr)game->handlePlayerChoice(socket,requestObject);

        break;
    default:
        qDebug() << "Received unknown request type:" << type;
        break;
    }
}

void Server::triggerNewGame()
{
    if (game != nullptr)
    {
        qDebug() << "Cannot start a new game, a game is already in progress.";
        return;
    }

    QTcpSocket* player1Socket = waitingQueue.takeFirst();
    QTcpSocket* player2Socket = waitingQueue.takeFirst();
    QString username1 = loggedInUsers.value(player1Socket);
    QString username2 = loggedInUsers.value(player2Socket);
    UserAccount* p1Account = &dataBase[username1];
    UserAccount* p2Account = &dataBase[username2];
    Player* player1 = new Player(p1Account, player1Socket, this);
    Player* player2 = new Player(p2Account, player2Socket, this);

    QList<Player*> gamePlayers;
    gamePlayers.append(player1);
    gamePlayers.append(player2);

    game = new Game( gamePlayers,this);
    QJsonObject responseToPlayer1;
    responseToPlayer1["type"] = START_GAME;
    responseToPlayer1["message"] = "SUCCESS2";
    responseToPlayer1["opponent_username"] = username2;
    player1Socket->write(QJsonDocument(responseToPlayer1).toJson());

    QJsonObject responseToPlayer2;
    responseToPlayer2["type"] = START_GAME;
    responseToPlayer2["message"] = "SUCCESS2";
    responseToPlayer2["opponent_username"] = username1;
    player2Socket->write(QJsonDocument(responseToPlayer2).toJson());
    connect(this->game, &Game::gameOver, this, &Server::onGameOver);
    game->startGame();
}

void Server::handleStartGame(const QJsonObject &request, QTcpSocket *socket)
{
    if (this->waitingQueue.contains(socket))
    {
        this->sendJsonReact(socket, START_GAME, "ALREADY_IN_QUEUE");
        return;
    }
    waitingQueue.append(socket);
    QString username = loggedInUsers.value(socket);
    qDebug() << "MATCHMAKING: user" << username << " queue size:" << this->waitingQueue.size();
    this->sendJsonReact(socket, START_GAME, "SUCCESS");
}

void Server::handleSignUp(const QJsonObject& request, QTcpSocket* socket){
    QString username = request["username"].toString();
    QString hashedPassword = request["password_hashed"].toString();
    QString email = request["email"].toString();
    QString phone = request["phone_number"].toString();
    QString name = request["name"].toString();
    QString surname = request["surname"].toString();

    if (this->dataBase.contains(username))
    {
        this->sendJsonReact(socket,SIGNUP,"USERNAME_TAKEN");
        return;
    }
    UserAccount newUser;
    newUser.setUsername(username);
    newUser.setHashedPassword(hashedPassword);
    newUser.setEmail(email);
    newUser.setPhoneNumber(phone);
    newUser.setFirstName(name);
    newUser.setLastName(surname);

    this->dataBase.insert(username,newUser);

    this->sendJsonReact(socket, SIGNUP, "SUCCESS");
    qDebug() <<"sinup success " << username;
    if (this->waitingQueue.size() >= 2)
    {
        emit gameReadyToStart();
    }

}

void Server::handleLogin(const QJsonObject &request, QTcpSocket *socket)
{
    QString username = request["username"].toString();
    QString hashedPassword = request["password_hashed"].toString();
    if (!this->dataBase.contains(username))
    {
        this->sendJsonReact(socket, LOGIN, "INVALID_CREDENTIALS");
        qDebug() << "username" << username << "not found.";
        return;
    }
    const UserAccount &user = this->dataBase.value(username);
    if(user.checkPassword(hashedPassword)){
        this->loggedInUsers.insert(socket,username);
        this->sendJsonReact(socket, LOGIN, "SUCCESS");
            qDebug()<< "login"<< username<<"successfully.";
    }
}

void Server::handleLogOut(const QJsonObject &request, QTcpSocket *socket)
{
    QString username = request["username"].toString();

    if (this->loggedInUsers.contains(socket))
    {
        QString username = this->loggedInUsers.value(socket);
        this->loggedInUsers.remove(socket);

        this->sendJsonReact(socket, LOGOUT, "SUCCESS");
        qDebug() << "LOGOUT: User" << username << "logged out successfully.";
    }
    else
    {
        this->sendJsonReact(socket, LOGOUT, "NOT_LOGGED_IN");
        qDebug() << "LOGOUT FAILED: The requesting socket was not logged in.";
    }
}

void Server::handleEditInfo(const QJsonObject &request, QTcpSocket *socket)
{
    if (!loggedInUsers.contains(socket))
    {
        sendJsonReact(socket, EDIT_INFO, "AUTH_REQUIRED");
        return;
    }
    QString currentUsername =this->loggedInUsers.value(socket);
    QString newUsername = request["username"].toString();
    if (currentUsername != newUsername && this->dataBase.contains(newUsername))
    {
        this->sendJsonReact(socket, EDIT_INFO, "USERNAME_TAKEN");
        qDebug() << "username" << currentUsername << " taken.";
        return;
    }


    QString hashedPassword = request["password_hashed"].toString();
    QString email = request["email"].toString();
    QString phone = request["phone_number"].toString();
    QString name = request["name"].toString();
    QString surname = request["surname"].toString();

    UserAccount &user = dataBase[currentUsername];
    user.setUsername(newUsername);
    user.setHashedPassword(hashedPassword);
    user.setEmail(email);
    user.setPhoneNumber(phone);
    user.setFirstName(name);
    user.setLastName(surname);

    this->dataBase.insert(newUsername, user);
    if(currentUsername != newUsername)
    {
        this->loggedInUsers[socket] = newUsername;
    }

    this->sendJsonReact(socket, EDIT_INFO, "SUCCESS");
    qDebug() << "EDIT_INFO: Profile updated for user" << newUsername;
}

void Server::handleForgetPassword(const QJsonObject &request, QTcpSocket *socket)
{
    QString phoneNumber = request["phone_number"].toString();
    QString newPassword = request["new_password"].toString();

    bool userFound = false;
    for (auto& user : dataBase)
    {
        if (user.getPhoneNumber() == phoneNumber)
        {
            user.setPassword(newPassword);
            userFound = true;
            break;
        }
    }

    if (userFound)
    {
        this->sendJsonReact(socket, FORGOT_PASSWORD, "SUCCESS");
        qDebug() << "FORGOT_PASSWORD: Password changed for phone number" << phoneNumber;
    }
    else
    {
        this->sendJsonReact(socket, FORGOT_PASSWORD, "PHONE_NOT_FOUND");
        qDebug() << "FORGOT_PASSWORD FAILED: Phone number" << phoneNumber << "not found.";
    }
}

void Server::handleGetAvailableCards(const QJsonObject& request, QTcpSocket* socket) {
    if (game == nullptr||loggedInUsers.contains(socket)) {
        qDebug() << "error in handle get available cards";
        return;
    }
    game->handleGetAvailableCards(socket);
}

void Server::sendJsonReact(QTcpSocket *socket, SERVER_CODES request, QString message)
{
    QJsonObject responseObject;

    responseObject["type"] = request;
    responseObject["message"] = message;

    QJsonDocument responseDoc(responseObject);
    if (socket && socket->isOpen())
    {
        socket->write(responseDoc.toJson());
    }
}

void Server::onGameOver()
{
    qDebug() << "Game has finished.";
    this->game->deleteLater();
    this->game = nullptr;
}

