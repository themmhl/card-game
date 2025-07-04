#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H
#include <QTcpSocket>
#include <QObject>
#include <QJsonArray>
#include "Card.h"
#include "Player.h"
using namespace std;

enum SERVER_CODES
{
    LOGIN, SIGNUP, LOGOUT, EDIT_INFO,
    FORGOT_PASSWORD, SHOW_HISTORY, START_GAME,
    GET_INITIAL_CARDS,
    GET_PLAYERS, SELECT_CARD, GET_AVAILABLE_CARDS, NEW_TURN, ROUND_RESULT, GAME_OVER, TIMEOUT_LOSE = 19,
    CHAT_REQUEST = 20, NEW_CHAT = 21, FLIP_REQUEST = 22, FLIP_REQUEST_RESULT = 23, FLIP_CHOICE=24
};

enum CARD_LOCATIONS
{
    MY_SELECTED, MY_AVAILABLE, OPP_SELECTED, OPP_AVAILABLE
};

enum WIN_TYPE
{
    MESSY_HAND,
    SINGLE_PAIR,
    DOUBLE_PAIR,
    THREE_OF_A_KIND,
    SERIES,
    MSC_HAND,
    PENTHOUSE_HAND,
    FOUR_OF_A_KIND,
    ORDER_HAND,
    GOLDEN_HAND,
    NO_RANK
};

class ServerHandler : public QObject
{
    Q_OBJECT
    int players_count = 2;
    QString ip_address;
    QString history = "";
    int port;
    QTcpSocket* socket;
    Player* player;
    pair<Player, Card[7]> current_player_cards[4];
    pair<Player, Card[7]> current_available_cards;
    int round_number = 0;
    bool read_done;
    int scores[4];

public:
    QString server_messages[12];
    explicit ServerHandler(QObject* parent);

public slots:
    void set_ip_address(const QString& ip_address);
    QString get_ip_address() const;
    void set_port(int port);
    QString get_server_message(int id);
    QString get_error();
    QString getPlayerName() const;
    QString getPlayerSurname() const;
    QString getPlayerEmail() const;
    QString getPlayerPhoneNumber() const;
    QString getPlayerUsername() const;
    QString getOppUsername() const;
    int get_score(int id);
    int get_port() const;
    bool connect_to_server();
    bool sign_up(QString, QString, QString, QString, QString, QString);
    bool login(QString username, QString password_raw);
    bool logout();
    bool forgot_password(QString, QString);
    bool edit_info(QString, QString, QString, QString, QString, QString);
    bool read_all(SERVER_CODES type);
    bool join_game();
    bool get_initial_cards();
    bool get_available_cards();
    bool select_card(Suit suit, Rank rank, SERVER_CODES type = SELECT_CARD);
    void read_available_cards(QJsonDocument doc);
    void read_cards_doc(QJsonDocument doc);
    void read_initial_cards_doc(QJsonDocument doc);
    bool get_players();
    void read_history_response(QJsonDocument doc);
    bool get_history();
    QString get_history_string();
    void new_turn(bool my_turn, bool initial_cards, bool end_round = false);
    bool flip_request();
    bool send_chat(const QString& content);
    bool timeout_lose();
signals:
    void status_changed(QString data, QString color);
    void add_card(int suit, int rank, CARD_LOCATIONS where);
    void clear();
    void show_status(QString title, QString context, QString color);
    void game_over(QString title, QString context, QString color);
    void start_timer();
    void stop_timer();
    void chat_received(QString text, bool mine);
    void flip_requested();
    void flip_request_accepted();
private slots:
    void readyRead();
    void onSocketError(QAbstractSocket::SocketError socketError);
};


#endif //SERVERHANDLER_H
