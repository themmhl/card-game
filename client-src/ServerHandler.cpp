#include "ServerHandler.h"
#include <QJsonObject>
#include <QCryptographicHash>

QString ServerHandler::get_ip_address() const
{
    return ip_address;
}

void ServerHandler::set_ip_address(const QString& ip_address)
{
    this->ip_address = ip_address;
}

int ServerHandler::get_port() const
{
    return port;
}

void ServerHandler::set_port(int port)
{
    this->port = port;
}

ServerHandler::ServerHandler(QObject* parent): QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::readyRead, this, &ServerHandler::readyRead);
    connect(socket, &QTcpSocket::error, this, &ServerHandler::error);
}

bool ServerHandler::connect_to_server()
{
    if (socket->isOpen()) socket->close();
    socket->connectToHost(ip_address, port);
    return socket->waitForConnected(-1);
}

bool ServerHandler::sign_up(QString name, QString surname, QString email, QString phone_number, QString username,
                            QString password)
{
    if (!socket->isOpen()) connect_to_server();
    QJsonObject signup_info;

    signup_info["type"] = SIGNUP;
    signup_info["name"] = name;
    signup_info["surname"] = surname;
    signup_info["email"] = email;
    signup_info["phone_number"] = phone_number;
    signup_info["username"] = username;
    signup_info["password_hashed"] = QString::fromStdString(QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
    ).toHex().toStdString());
    QJsonDocument doc(signup_info);

    socket->write(doc.toJson());

    return read_all(SIGNUP);
}

bool ServerHandler::login(QString username, QString password_raw)
{
    if (!socket->isOpen()) connect_to_server();
    QJsonObject login_info;
    login_info["type"] = LOGIN;
    login_info["username"] = username;
    login_info["password"] = password_raw;
    QJsonDocument doc(login_info);
    socket->write(doc.toJson());
    return read_all(LOGIN);
}

bool ServerHandler::logout()
{
    QJsonObject logout_info;
    logout_info["type"] = LOGOUT;
    QJsonDocument doc(logout_info);
    socket->write(doc.toJson());
    delete player;
    return true;
}

bool ServerHandler::forgot_password(QString phone_num, QString new_pass)
{
    if (!socket->isOpen()) connect_to_server();
    QJsonObject forgot_pass_info;
    forgot_pass_info["type"] = FORGOT_PASSWORD;
    forgot_pass_info["phone_number"] = phone_num;
    forgot_pass_info["new_password"] = new_pass;
    QJsonDocument doc(forgot_pass_info);
    socket->write(doc.toJson());
    return read_all(FORGOT_PASSWORD);
}

bool ServerHandler::edit_info(QString name, QString surname, QString email, QString phone_number, QString username,
                              QString password)
{
    QJsonObject edit_info;

    edit_info["type"] = EDIT_INFO;
    edit_info["name"] = name;
    edit_info["surname"] = surname;
    edit_info["email"] = email;
    edit_info["phone_number"] = phone_number;
    edit_info["username"] = username;
    edit_info["password_hashed"] = QString::fromStdString(QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
    ).toHex().toStdString());
    QJsonDocument doc(edit_info);

    socket->write(doc.toJson());

    return read_all(EDIT_INFO);
}

bool ServerHandler::read_all(SERVER_CODES type)
{
    if (!socket->waitForReadyRead(5000))
    {
        server_messages[type] = "Server Timeout.";
        return false;
    }

    read_done = true;
    QJsonObject reply = QJsonDocument::fromJson(socket->readAll()).object();
    server_messages[reply["type"].toInt()] = reply["message"].toString();

    if (reply["type"].toInt() == type)
    {
        return server_messages[type] == "SUCCESS";
    }
    server_messages[type] = "Invalid Server Response.";
    return false;
}

void ServerHandler::readyRead()
{
    if (read_done)
    {
        read_done = false;
        return;
    }
    QJsonObject reply = QJsonDocument::fromJson(socket->readAll()).object();
    server_messages[reply["type"].toInt()] = reply["message"].toString();
}

void ServerHandler::error()
{
}
