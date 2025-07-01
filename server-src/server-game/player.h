#ifndef PLAYER_H
#define PLAYER_H
#include <QObject>
#include <QTcpSocket>
#include "useraccount.h"
#include "card.h"
#include <QVector>

class Player : public QObject {
    Q_OBJECT;
    QTcpSocket* clientSocket;
    QVector<Card> Hand;
    int playerScore;
    bool myTurn;
public:
    UserAccount* userAccount;
    explicit Player(UserAccount* account, QTcpSocket* socket, QObject *parent = nullptr);

    QTcpSocket* getSocket() const;
    QString getUsername() const;
    UserAccount* getAccount() const;
    QVector<Card> getHand() const;
    int getScore() const;

    bool isMyTurn() const;
    void setMyTurn(bool isTurn);
    void setHand(const QVector<Card>& hand);
    void addCard(const Card& card);
    void setPlayerScore(int score);
    void appendPlayerScore(int score);

};
#endif //PLAYER_H
