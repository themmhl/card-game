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
    // connect(socket, &QTcpSocket::error, this, &ServerHandler::error);
    connect(socket, &QTcpSocket::errorOccurred, this, &ServerHandler::onSocketError);
}

bool ServerHandler::connect_to_server()
{
    if (socket->isOpen()) socket->close();
    socket->connectToHost(ip_address, port);
    if (!socket->waitForConnected(-1))
    {
        return false;
    }
    return true;
}

QString ServerHandler::get_server_message(int id)
{
    return server_messages[id];
}

QString ServerHandler::get_error()
{
    return socket->errorString();
}

int ServerHandler::get_score(int id)
{
    return scores[id];
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
    bool result = read_all(LOGIN);
    if (result)
    {
        if (!player)
        {
            player = new Player();
        }
        player->username = username;
    }
    return result;
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


bool ServerHandler::join_game()
{
    QJsonObject join_game_info;
    join_game_info["type"] = START_GAME; //its a enum which is an integer of 6
    QJsonDocument doc(join_game_info);
    socket->write(doc.toJson());
    return read_all(START_GAME); //server responds with "SUCCESS"
}

bool ServerHandler::get_initial_cards()
{
    QJsonObject join_game_info;
    join_game_info["type"] = GET_INITIAL_CARDS;
    QJsonDocument doc(join_game_info);
    socket->write(doc.toJson());
    if (!socket->waitForReadyRead(5000))
    {
        return false;
    }
    read_done = true;
    auto reply = QJsonDocument::fromJson(socket->readAll());
    read_cards_doc(reply);
    return true;
}

bool ServerHandler::get_available_cards()
{
    QJsonObject available_cards_info;
    available_cards_info["type"] = GET_AVAILABLE_CARDS;
    QJsonDocument doc(available_cards_info);
    socket->write(doc.toJson());
    if (!socket->waitForReadyRead(5000))
    {
        return false;
    }
    read_done = true;
    auto reply = QJsonDocument::fromJson(socket->readAll());
    read_available_cards(reply);
    return true;
}

bool ServerHandler::select_card(Suit suit, Rank rank)
{
    QJsonObject select_card_info;
    select_card_info["type"] = SELECT_CARD;
    select_card_info["suit"] = suit;
    select_card_info["rank"] = rank;
    QJsonDocument doc(select_card_info);
    socket->write(doc.toJson());
    return read_all(SELECT_CARD);
}

void ServerHandler::read_available_cards(QJsonDocument doc)
{
    QJsonObject root = doc.object();
    if (!root.contains("payload") || !root["payload"].isArray()) return;

    QJsonArray payload = root["payload"].toArray();
    if (payload.isEmpty()) return;

    QJsonObject entry = payload[0].toObject();

    QJsonObject playerObj = entry["player"].toObject();
    current_available_cards.first.username = playerObj["username"].toString();
    current_available_cards.first.turn = playerObj["turn"].toBool();

    QJsonArray handArray = entry["hand"].toArray();
    for (int i = 0; i < 7 && i < handArray.size(); ++i)
    {
        QJsonObject cardObj = handArray[i].toObject();
        current_available_cards.second[i].cardSuit = static_cast<Suit>(cardObj["suit"].toInt());
        current_available_cards.second[i].cardRank = static_cast<Rank>(cardObj["rank"].toInt());
    }
}


void ServerHandler::read_cards_doc(QJsonDocument doc)
{
    QJsonObject root = doc.object();

    QJsonArray payload = root["payload"].toArray();

    for (int i = 0; i < players_count && i < payload.size(); ++i)
    {
        QJsonObject entry = payload[i].toObject();

        QJsonObject playerObj = entry["player"].toObject();
        current_player_cards[i].first.username = playerObj["username"].toString();
        current_player_cards[i].first.turn = playerObj["turn"].toBool();
        QJsonArray handArray = entry["hand"].toArray();
        for (int j = 0; j < 7 && j < handArray.size(); ++j)
        {
            QJsonObject cardObj = handArray[j].toObject();
            current_player_cards[i].second[j].cardSuit = static_cast<Suit>(cardObj["suit"].toInt());
            current_player_cards[i].second[j].cardRank = static_cast<Rank>(cardObj["rank"].toInt());
        }
    }
}


bool ServerHandler::get_players()
{
    QJsonObject join_game_info;
    join_game_info["type"] = GET_PLAYERS;
    QJsonDocument doc(join_game_info);
    socket->write(doc.toJson());
    if (!socket->waitForReadyRead(5000))
    {
        return false;
    }
    read_done = true;
    auto reply = QJsonDocument::fromJson(socket->readAll());
    read_cards_doc(reply);
    return true;
}


bool ServerHandler::read_all(SERVER_CODES type)
{
    if (!socket->waitForReadyRead(5000))
    {
        server_messages[type] = "Server Timeout.";
        qInfo() << "timeout - id: " << type;
        qInfo() << "what happened? " << socket->errorString();
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

void ServerHandler::new_turn(const bool my_turn)
{
    emit clear();
    get_available_cards();
    get_players();
    for (auto card : current_available_cards.second)
    {
        emit add_card(card.cardSuit, card.cardRank, my_turn ? MY_AVAILABLE : OPP_AVAILABLE);
    }
    int i = current_player_cards[0].first.username == player->username ? MY_SELECTED : OPP_SELECTED;
    for (auto card : current_player_cards[0].second)
    {
        emit add_card(card.cardSuit, card.cardRank, static_cast<CARD_LOCATIONS>(i));
    }
    i = current_player_cards[1].first.username == player->username ? MY_SELECTED : OPP_SELECTED;
    for (auto card : current_player_cards[1].second)
    {
        emit add_card(card.cardSuit, card.cardRank, static_cast<CARD_LOCATIONS>(i));
    }
    if (round_number++ == 0)
    {
        if (current_player_cards[0].second[0].cardRank>current_player_cards[1].second[0].cardRank)
        {
            emit show_status("You are the starter.");
        }
        else
        {
            emit show_status(current_player_cards[1].first.name + " is the starter.");
        }
    }
    if (round_number++ == 10)
    {
        if (!socket->waitForReadyRead(5000))
        {
            //handle error
        }

        read_done = true;
        QJsonObject reply = QJsonDocument::fromJson(socket->readAll()).object();
        if (reply["type"].toInt() == ROUND_RESULT)
        {
            QString status_text = "";
            if (reply["winner_username"].toString() == player->username)
            {
                scores[0]++;
                status_text = "You Win!";
            }
            else
            {
                scores[1]++;
                status_text = "You Lose!";
            }
            status_text += "\nWinning hand type: ";
            switch (reply["winning_hand_type"].toInt())
            {
            case MESSY_HAND:
                status_text += "Messy Hand";
                break;
            case SINGLE_PAIR:
                status_text += "Single Pair";
                break;
            case DOUBLE_PAIR:
                status_text += "Double Pair";
                break;
            case THREE_OF_A_KIND:
                status_text += "Three of a Kind";
                break;
            case SERIES:
                status_text += "Series";
                break;
            case MSC_HAND:
                status_text += "MSC Hand";
                break;
            case PENTHOUSE_HAND:
                status_text += "Penthouse Hand";
                break;
            case FOUR_OF_A_KIND:
                status_text += "Four of a Kind";
                break;
            case ORDER_HAND:
                status_text += "Ordered Hand";
                break;
            case GOLDEN_HAND:
                status_text += "Golden Hand";
                break;
            default:
                status_text += "No rank";
            }
            emit show_status(status_text);
        }
    }
}

void ServerHandler::readyRead()
{
    if (read_done)
    {
        qInfo() << "read_done is reset.";
        read_done = false;
        return;
    }
    QJsonObject reply = QJsonDocument::fromJson(socket->readAll()).object();
    if (reply["type"].toInt() == NEW_TURN)
    {
        new_turn(player->username == reply["username"]);
    }
    else
    {
        server_messages[reply["type"].toInt()] = reply["message"].toString();
        if (reply["message"].toString() == "SUCCESS2")
        {
            emit status_changed("Opponent found. Starting match...", "green");
        }
    }
}

void ServerHandler::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "🔥 Socket error occurred:" << socketError;
    switch (socketError)
    {
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "🛑 Host not found.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "🛑 Connection refused by server.";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() << "🔌 Remote host closed the connection.";
        break;
    default:
        qDebug() << "⚠️ Unhandled socket error.";
        break;
    }
}
