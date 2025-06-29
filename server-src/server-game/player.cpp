#include "player.h"


Player::Player(UserAccount* account, QTcpSocket* socket, QObject *parent ):QObject(parent) {
    this->userAccount = account;
    this->clientSocket = socket;
}

QTcpSocket* Player::getSocket() const {
    return this->clientSocket;
}
QString Player::getUsername() const {
    return this->userAccount->getUsername();
}
UserAccount* Player::getAccount() const {
    return this->userAccount;
}

void Player::setHand(const QVector<Card>& hand) {
    Hand = hand;
}
QVector<Card> Player::getHand() const {
    return Hand;
}