#include "ServerHandler.h"
#include <QJsonObject>
#include <QCryptographicHash>
#include <QThread>

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
    for (auto shit : current_available_cards.second)
    {
        shit.cardSuit = static_cast<Suit>(10);
    }
    scores[0] = 0;
    scores[1] = 0;
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
    qInfo() << "signup pass: " << signup_info["password_hashed"];
    socket->write(doc.toJson());

    return read_all(SIGNUP);
}

bool ServerHandler::login(QString username, QString password_raw)
{
    if (!socket->isOpen()) connect_to_server();
    QJsonObject login_info;
    login_info["type"] = LOGIN;
    login_info["username"] = username;
    login_info["password_hashed"] = QString::fromStdString(QCryptographicHash::hash(
        password_raw.toUtf8(),
        QCryptographicHash::Sha256
    ).toHex().toStdString());
    qInfo() << "given pass: " << login_info["password_hashed"];
    QJsonDocument doc(login_info);
    socket->write(doc.toJson());
    bool result = read_all(LOGIN);
    if (result)
    {
        player = new Player();
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
    qInfo() << "asked for initial cards";
    QJsonObject join_game_info;
    join_game_info["type"] = GET_INITIAL_CARDS;
    QJsonDocument doc(join_game_info);
    socket->write(doc.toJson());
    if (!socket->waitForReadyRead(5000))
    {
        return false;
    }
    read_done = true;
    QByteArray jsonData = socket->readLine().trimmed();
    if (jsonData.isEmpty())
    {
        if (socket->waitForReadyRead(100))
        {
            jsonData = socket->readLine().trimmed();
        }
        if (jsonData.isEmpty())
        {
            qWarning() << "get_initial_cards: Still empty after short wait.";
            return false;
        }
    }
    doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject())
    {
        qWarning() << "get_initial_cards: Failed to parse JSON or not an object. Data:" << jsonData;
        return false;
    }
    read_cards_doc(doc);
    return true;
}

bool ServerHandler::get_available_cards()
{
    qInfo() << "asked for available cards";
    QJsonObject available_cards_info;
    available_cards_info["type"] = GET_AVAILABLE_CARDS;
    QJsonDocument doc(available_cards_info);
    socket->write(doc.toJson());
    if (!socket->waitForReadyRead(5000))
    {
        return false;
    }
    read_done = true;
    QThread::msleep(100);
    QByteArray jsonData = socket->readLine().trimmed();
    if (jsonData.isEmpty())
    {
        qWarning() << "get_available_cards: Received empty response from server.";
        if (socket->waitForReadyRead(100))
        {
            jsonData = socket->readLine().trimmed();
        }
        if (jsonData.isEmpty())
        {
            qWarning() << "get_available_cards: Still empty after short wait.";
            return false;
        }
    }
    doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject())
    {
        qWarning() << "get_available_cards: Failed to parse JSON or not an object. Data:" << jsonData;
        return false;
    }

    qInfo() << "get_available_cards: " << doc.toJson();
    read_available_cards(doc);
    return true;
}

bool ServerHandler::select_card(Suit suit, Rank rank, SERVER_CODES type)
{
    emit stop_timer();
    QJsonObject select_card_info;
    select_card_info["type"] = type;
    select_card_info["suit"] = suit;
    select_card_info["rank"] = rank;
    QJsonDocument doc(select_card_info);
    QByteArray out = doc.toJson(QJsonDocument::Compact) + "\n";
    socket->write(out);
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

    int actualCardCount = handArray.size();

    for (int i = 0; i < actualCardCount && i < 7; ++i)
    {
        QJsonObject cardObj = handArray[i].toObject();
        current_available_cards.second[i].cardSuit = static_cast<Suit>(cardObj["suit"].toInt());
        current_available_cards.second[i].cardRank = static_cast<Rank>(cardObj["rank"].toInt());
    }

    // Fill rest with dummy ass cards
    for (int i = actualCardCount; i < 7; ++i)
    {
        current_available_cards.second[i].cardSuit = static_cast<Suit>(10); // goofy suit
        current_available_cards.second[i].cardRank = static_cast<Rank>(0);
    }
}


void ServerHandler::read_initial_cards_doc(QJsonDocument doc)
{
    QJsonObject root = doc.object();

    if (!root.contains("payload")) return;
    QJsonObject payload = root["payload"].toObject();

    if (!payload.contains("players_initial_cards")) return;
    QJsonArray playersArray = payload["players_initial_cards"].toArray();

    for (int i = 0; i < players_count; ++i)
    {
        if (i < playersArray.size())
        {
            QJsonObject playerEntry = playersArray[i].toObject();

            QString username = playerEntry["username"].toString();
            QJsonObject cardObj = playerEntry["initial_card"].toObject();
            Suit suit = static_cast<Suit>(cardObj["suit"].toInt());
            Rank rank = static_cast<Rank>(cardObj["rank"].toInt());

            current_player_cards[i].first.username = username;

            current_player_cards[i].second[0].cardSuit = DIAMOND;
            current_player_cards[i].second[0].cardRank = rank;

            // Fill rest of cards 1 to 6 with dummy shit = 10
            for (int j = 1; j < 7; ++j)
            {
                current_player_cards[i].second[j].cardSuit = static_cast<Suit>(10);
                current_player_cards[i].second[j].cardRank = static_cast<Rank>(0);
            }
        }
        else
        {
            current_player_cards[i].first.username.clear();

            for (int j = 0; j < 7; ++j)
            {
                current_player_cards[i].second[j].cardSuit = static_cast<Suit>(10);
                current_player_cards[i].second[j].cardRank = static_cast<Rank>(0);
            }
        }
    }

}


void ServerHandler::read_cards_doc(QJsonDocument doc)
{
    QJsonObject root = doc.object();
    QJsonArray payload = root["payload"].toArray();

    for (int i = 0; i < 2 && i < payload.size(); ++i)
    {
        QJsonObject entry = payload[i].toObject();

        QJsonObject playerObj = entry["player"].toObject();
        current_player_cards[i].first.username = playerObj["username"].toString();
        current_player_cards[i].first.turn = playerObj["turn"].toBool();

        QJsonArray handArray = entry["hand"].toArray();

        for (int j = 0; j < 7; ++j)
        {
            if (j < handArray.size())
            {
                QJsonObject cardObj = handArray[j].toObject();
                current_player_cards[i].second[j].cardSuit = static_cast<Suit>(cardObj["suit"].toInt());
                current_player_cards[i].second[j].cardRank = static_cast<Rank>(cardObj["rank"].toInt());
            }
            else
            {
                current_player_cards[i].second[j].cardSuit = static_cast<Suit>(10);
            }
        }
    }
}


bool ServerHandler::get_players()
{
    QJsonObject join_game_info;
    join_game_info["type"] = GET_PLAYERS;
    QJsonDocument doc(join_game_info);
    QByteArray out = doc.toJson(QJsonDocument::Compact) + "\n";
    socket->write(out);
    if (!socket->waitForReadyRead(5000))
    {
        history = "Error loading history. Server Timeout.";
        return false;
    }
    read_done = true; // To signal readyRead to ignore this upcoming message
    QByteArray jsonData = socket->readLine().trimmed();
    if (jsonData.isEmpty())
    {
        qWarning() << "get_players: Received empty response from server.";
        history = "Error loading history. Empty response.";
        if (socket->waitForReadyRead(100))
        {
            jsonData = socket->readLine().trimmed();
        }
        if (jsonData.isEmpty())
        {
            qWarning() << "get_players: Still empty after short wait.";
            return false;
        }
    }
    doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isObject())
    {
        qWarning() << "get_players: Failed to parse JSON or not an object. Data:" << jsonData;
        history = "Error loading history. Invalid JSON.";
        return false;
    }
    qInfo() << "got the get_players doc: " << doc;
    read_cards_doc(doc);
    return true;
}

void ServerHandler::read_history_response(QJsonDocument doc)
{
    QJsonObject root = doc.object();

    history.clear();

    if (!root.contains("payload") || !root["payload"].isArray())
    {
        history = "No recent games.";
        return;
    }

    QJsonArray payload = root["payload"].toArray();
    if (payload.isEmpty())
    {
        history = "No recent games.";
        return;
    }

    for (int i = 0; i < payload.size() && i < 3; ++i)
    {
        QJsonObject game = payload[i].toObject();

        QString opp = game["opp_username"].toString().leftJustified(15, ' ');
        QString date = game["date"].toString().leftJustified(15, ' ');
        QString rounds = game["rounds"].toString().leftJustified(10, ' ');
        QString result = game["result"].toString().leftJustified(6, ' ');

        QString line = QString("%1%2%3%4").arg(opp, date, rounds, result);
        history += line + "\n";
    }
}

bool ServerHandler::get_history()
{
    QJsonObject show_history_info;
    show_history_info["type"] = SHOW_HISTORY;
    show_history_info["username"] = player->username;
    QJsonDocument doc(show_history_info);
    QByteArray out = doc.toJson(QJsonDocument::Compact) + "\n";
    socket->write(out);
    if (!socket->waitForReadyRead(5000))
    {
        return false;
    }
    read_done = true;
    auto reply = QJsonDocument::fromJson(socket->readAll());
    read_history_response(reply);
    return true;
}

QString ServerHandler::get_history_string()
{
    return history;
}

QString ServerHandler::getPlayerName() const
{
    return player ? player->name : "";
}

QString ServerHandler::getPlayerSurname() const
{
    return player ? player->surname : "";
}

QString ServerHandler::getPlayerEmail() const
{
    return player ? player->email : "";
}

QString ServerHandler::getPlayerPhoneNumber() const
{
    return player ? player->phone_number : "";
}

QString ServerHandler::getPlayerUsername() const
{
    return player ? player->username : "";
}

QString ServerHandler::getOppUsername() const
{
    if (current_player_cards[0].first.username == player->username)
    {
        return current_player_cards[1].first.username;
    }
    return current_player_cards[0].first.username;
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
    QByteArray jsonData;

    for (int retry = 0; retry < 5; ++retry)
    {
        jsonData = socket->readLine().trimmed();

        if (jsonData.isEmpty())
        {
            if (socket->waitForReadyRead(100))
                jsonData = socket->readLine().trimmed();
            if (jsonData.isEmpty()) continue;
        }

        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        if (doc.isNull() || !doc.isObject())
        {
            qWarning() << "Invalid JSON in read_all: " << jsonData;
            continue;
        }

        QJsonObject reply = doc.object();
        int replyType = reply["type"].toInt();

        qInfo() << "read_all got type " << replyType << ", expecting " << type;

        if (replyType == type)
        {
            server_messages[type] = reply["message"].toString();
            return server_messages[type] == "SUCCESS";
        }
        else
        {
            read_done = false;
            // put back extra shit for readyRead to handle
            QMetaObject::invokeMethod(this, [this, jsonData]()
            {
                socket->write(jsonData + "\n");
            }, Qt::QueuedConnection);
            //QThread::msleep(100);
        }
    }

    server_messages[type] = "Invalid Server Response or Timeout.";
    return false;
}


void ServerHandler::new_turn(const bool my_turn, bool initial_cards, bool end_round)
{
    for (auto shit : current_available_cards.second)
    {
        shit.cardSuit = static_cast<Suit>(10);
    }
    for (auto shit : current_player_cards[0].second)
    {
        shit.cardSuit = static_cast<Suit>(10);
    }
    for (auto shit : current_player_cards[1].second)
    {
        shit.cardSuit = static_cast<Suit>(10);
    }
    QThread::msleep(100);

    if (!initial_cards && !end_round)
    {
        get_available_cards();
        get_players();
    }
    qInfo() << "new turn nigga, initial_cards: " << initial_cards;
    qInfo() << "new turn nigga, is it my turn? " << my_turn;
    emit clear();
    if (my_turn)
    {
        qInfo() << "start timer";
        emit start_timer();
    }
    for (auto card : current_available_cards.second)
    {
        if (card.cardSuit <= 4 && !initial_cards && !end_round)
        {
            emit add_card(card.cardSuit, card.cardRank, my_turn ? MY_AVAILABLE : OPP_AVAILABLE);
        }
    }
    int i = current_player_cards[0].first.username == player->username ? MY_SELECTED : OPP_SELECTED;
    for (auto card : current_player_cards[0].second)
    {
        if (card.cardSuit <= 4)
        {
            qInfo() << "added a card to current player shit: , rank: " << card.getRank() << " , suit: " << card.
                getSuit();
            emit add_card(initial_cards ? 0 : card.cardSuit, card.cardRank, static_cast<CARD_LOCATIONS>(i));
        }
    }
    i = current_player_cards[1].first.username == player->username ? MY_SELECTED : OPP_SELECTED;
    for (auto card : current_player_cards[1].second)
    {
        if (card.cardSuit <= 4)
        {
            qInfo() << "added a card to another current player shit: , rank: " << card.getRank() << " , suit: " << card.
                getSuit();
            emit add_card(initial_cards ? 0 : card.cardSuit, card.cardRank, static_cast<CARD_LOCATIONS>(i));
        }
    }
    if (!end_round)
    {
        if (round_number++ == 0)
        {
            i = current_player_cards[0].first.username == player->username ? 0 : 1;
            if (current_player_cards[i].second[0].cardRank > current_player_cards[1 - i].second[0].cardRank)
            {
                emit show_status("You are the starter.", "", "#b345ff00");
            }
            else
            {
                emit show_status(current_player_cards[1 - i].first.username + " is the starter.", "", "#b3aa0000");
            }
        }
        if (round_number == 10)
        {
            //if (!socket->waitForReadyRead(5000))
            {
                //handle error
            }

            //r//ead_done = true;
            //QJsonObject reply = QJsonDocument::fromJson(socket->readAll()).object();
        }
    }
}

bool ServerHandler::flip_request()
{
    QJsonObject obj;
    obj["type"] = FLIP_REQUEST;

    QJsonDocument doc(obj);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");

    return true;
}

bool ServerHandler::send_chat(const QString& content)
{
    QJsonObject obj;
    obj["type"] = CHAT_REQUEST;
    QJsonObject payload;
    payload["content"] = content;
    obj["payload"] = payload;

    QJsonDocument doc(obj);
    socket->write(doc.toJson(QJsonDocument::Compact) + "\n");

    return true;
}

bool ServerHandler::timeout_lose()
{
    QJsonObject timeout_info;
    timeout_info["type"] = TIMEOUT_LOSE; //its a enum which is an integer of 19
    QJsonDocument doc(timeout_info);
    socket->write(doc.toJson());
    return read_all(TIMEOUT_LOSE);
}

void ServerHandler::readyRead()
{
    if (read_done)
    {

        QByteArray peekData = socket->peek(4096);
        QList<QByteArray> lines = peekData.split('\n');

        for (const QByteArray& line : lines)
        {
            if (line.trimmed().isEmpty()) continue;

            QJsonDocument peekDoc = QJsonDocument::fromJson(line);
            if (!peekDoc.isObject()) continue;

            QJsonObject peekObj = peekDoc.object();
            int msgType = peekObj["type"].toInt();

            if (msgType == NEW_TURN || msgType == NEW_CHAT || msgType == GET_INITIAL_CARDS || msgType == GAME_OVER ||
                msgType == ROUND_RESULT || msgType == FLIP_REQUEST_RESULT||msgType == FLIP_REQUEST
                || peekObj["message"].toString() == "SUCCESS2")
            {
                break; // let readyRead handle it below
            }
            else
            {
                read_done = false; // so future readyRead calls work
                return;
            }
        }
        read_done = false;
    }
    qInfo() << "we are in readyread.";
    while (socket->canReadLine())
    {
        QByteArray jsonData = socket->readLine().trimmed();
        if (jsonData.isEmpty())
        {
            continue; // Skip empty lines
        }
        QJsonDocument doc = QJsonDocument::fromJson(jsonData);
        QJsonObject reply = doc.object();
        qInfo() << "new reply in readyread: " << reply;
        if (reply.isEmpty())
        {
            jsonData = socket->readAll();
            doc = QJsonDocument::fromJson(jsonData);
            reply = doc.object();
            //continue;
        }
        if (reply["type"].toInt() == NEW_TURN)
        {
            qInfo() << "NEW_TURN received. reply username:" << reply["username"].toString();

            new_turn(player->username == reply["username"], false);
        }
        else if (reply["type"].toInt() == FLIP_REQUEST)
        {
            emit flip_requested();
        }
        else if (reply["type"].toInt() == FLIP_REQUEST_RESULT)
        {
            if (reply["message"].toString() == "SUCCESS")
            {
                emit flip_request_accepted();
            }
        }
        else if (reply["type"].toInt() == GET_INITIAL_CARDS)
        {
            qInfo() << "GET_INITIAL_CARDS received. reply username:" << reply["username"].toString();

            read_initial_cards_doc(doc);
            new_turn(player->username == reply["username"], true);
        }
        else if (reply["type"].toInt() == ROUND_RESULT)
        {
            QJsonObject payload = reply["payload"].toObject();

            QString status_text = "";
            QString title_text = "";
            QString winner = payload["winner_username"].toString();

            bool win = winner == player->username;

            if (win)
            {
                scores[0]++;
                title_text = "You Win!";
            }
            else
            {
                scores[1]++;
                title_text = "You Lose!";
            }

            status_text += "Winning hand type: ";
            switch (payload["winning_hand_type"].toInt())
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
            QJsonArray handsArray = payload["all_players_hands"].toArray();

            QJsonArray fakePayload;
            for (const auto& playerVal : handsArray)
            {
                QJsonObject player = playerVal.toObject();
                QJsonObject entry;

                QJsonObject playerMeta;
                playerMeta["username"] = player["username"].toString();
                playerMeta["turn"] = false; // ain't matter in this context

                entry["player"] = playerMeta;
                entry["hand"] = player["hand"].toArray();

                fakePayload.append(entry);
            }

            QJsonObject fakeRoot;
            fakeRoot["payload"] = fakePayload;
            QJsonDocument doc(fakeRoot);

            read_cards_doc(doc);

            for (auto shit : current_available_cards.second)
            {
                shit.cardSuit = static_cast<Suit>(10);
            }
            new_turn(true, false, true);
            emit show_status(title_text, status_text, win ? "#b345ff00" : "#b3ff0000");

            //handle this shit: QJsonObject({"payload":{"all_players_hands":[{"hand":[{"rank":6,"suit":0},{"rank":11,"suit":0},{"rank":12,"suit":3},{"rank":5,"suit":0},{"rank":11,"suit":3}],"username":"1"},{"hand":[{"rank":3,"suit":0},{"rank":6,"suit":1},{"rank":8,"suit":3},{"rank":1,"suit":3},{"rank":11,"suit":2}],"username":"2"}],"current_scores":{"1":1,"2":0},"winner_username":"1","winning_hand_type":1},"type":12})
        }
        else if (reply["type"].toInt() == GAME_OVER)
        {
            QJsonObject payload = reply["payload"].toObject();
            //handle gameover shit
            if (payload["final_winner_username"].toString() == player->username)
            {
                emit game_over("You Win!", "", "#b345ff00");
            }
            else
            {
                emit game_over("You Lost!", "", "#b3ff0000");
            }
            scores[0] = 0;
            scores[1] = 0;
        }
        else if (reply["type"].toInt() == NEW_CHAT)
        {
            QJsonObject payload = reply["payload"].toObject();
            QString sender = payload["sender"].toString();
            QString content = payload["content"].toString();

            qInfo() << " Chat received from" << sender << ": " << content;

            emit chat_received(content, sender == player->username);
        }
        else
        {
            server_messages[reply["type"].toInt()] = reply["message"].toString();
            if (reply["message"].toString() == "SUCCESS2")
            {
                qInfo() << "Opponent found from readyread. Starting match...";
                emit status_changed("Opponent found. Starting match...", "green");
                //QThread::msleep(200);
            }
        }
    }
}

void ServerHandler::onSocketError(QAbstractSocket::SocketError socketError)
{
    qDebug() << "Socket error occurred:" << socketError;
    switch (socketError)
    {
    case QAbstractSocket::HostNotFoundError:
        qDebug() << "Host not found.";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug() << "connection refused by server.";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        qDebug() << "Remote host closed the connection.";
        break;
    default:
        qDebug() << "Unhandled socket error.";
        break;
    }
}
