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
    int notChoosingCardCount;
    int pauseRequestingCount;
    bool myTurn;
public:
    UserAccount* userAccount;
    explicit Player(UserAccount* account, QTcpSocket* socket, QObject *parent = nullptr);

    QTcpSocket* getSocket() const;
    QString getUsername() const;
    UserAccount* getAccount() const;
    QVector<Card> getHand() const;
    int getScore() const;
    void clearHand();
    bool isMyTurn() const;
    void setMyTurn(bool isTurn);
    void setHand(const QVector<Card>& hand);
    void addCard(const Card& card);
    void setPlayerScore(int score);
    void appendPlayerScore(int score);
    bool minusPauseRequestingCount();
    bool minusNotChoosingCardCount();
    QVector<Card>& getReferenceHand() {
        return Hand;
    }
};
#endif //PLAYER_H
