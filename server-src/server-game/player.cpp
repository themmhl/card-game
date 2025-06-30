#include "player.h"


Player::Player(UserAccount* account, QTcpSocket* socket, QObject *parent ):QObject(parent) {
    userAccount = account;
    clientSocket = socket;
    playerScore = 0;
    myTurn = false;
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

int Player::getScore() const{
    return playerScore;
}

void Player::setPlayerScore(int score) {
    playerScore = score;
}

void Player::appendPlayerScore(int score) {
    playerScore += score;
}

bool Player::isMyTurn() const {
    return myTurn;

}
void Player::setMyTurn(bool isTurn) {
    myTurn = isTurn;
}

void Player::addCard(const Card& card) {
    Hand.push_back(card);
}