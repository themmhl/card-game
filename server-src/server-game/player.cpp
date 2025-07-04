#include "player.h"


Player::Player(UserAccount* account, QTcpSocket* socket, QObject *parent ):QObject(parent) {
    userAccount = account;
    clientSocket = socket;
    playerScore = 0;
    pauseRequestingCount = 2;
    notChoosingCardCount = 2;
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

void Player::clearHand()
{
    Hand.clear();
}

void Player::setPlayerScore(int score) {
    playerScore = score;
}

void Player::appendPlayerScore(int score) {
    playerScore += score;
}

bool Player::minusPauseRequestingCount() {
    //return pauseRequestingCount-->0? true: false;
    pauseRequestingCount--;
    if (pauseRequestingCount>=0) {
        return true;
    }
    return false;
}

bool Player::minusNotChoosingCardCount() {
    //return notChoosingCardCount-->0? true: false;
    notChoosingCardCount--;
    if (notChoosingCardCount>=0) {
        return true;
    }
    return false;
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