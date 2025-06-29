

#ifndef PLAYER_H
#define PLAYER_H
#include <QObject>
#include <QTcpSocket>
#include "useraccount.h"
#include "card.h"
#include <QVector>

class Player : public QObject {
    Q_OBJECT;
    UserAccount* userAccount;
    QTcpSocket* clientSocket;
    QVector<Card> Hand;
public:
    explicit Player(UserAccount* account, QTcpSocket* socket, QObject *parent = nullptr);

    QTcpSocket* getSocket() const;
    QString getUsername() const;
    UserAccount* getAccount() const;

    void setHand(const QVector<Card>& hand);
    QVector<Card> getHand() const;

};
#endif //PLAYER_H
