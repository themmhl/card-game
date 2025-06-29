#include "game.h"
#include <QJsonObject>
#include <QJsonDocument>

Game::Game(const QList<Player*>& players, QObject *parent ) :QObject(parent){
    gamePlayers = players;
    currentRound = 0;
    startingPlayerIndex = 0;
    currentState = PENDING;
    qDebug() << "created game with"<<players.count();
}

void Game::startGame() {
    qDebug() << "game is starting!";
    currentState = ROUND_STARTING;
    startNewRound();
}

QList<Player*> Game::getPlayers() const {
    return gamePlayers;
}

void Game::startNewRound() {
    currentRound++;
    qDebug() << "start new round"<<currentRound;
    deck = HandleCard();
    deck.shuffle();

    dealCardsForTurn();
}

QVector<Card> Game::dealCardsForTurn() {
    currentState = DEALING_CARDS;
    Player* startingPlayer = gamePlayers[startingPlayerIndex];
    startingPlayer->setMyTurn(true);

    QVector<Card> handToPickForm;
    for (int i = 0;i<7;i++) {
        handToPickForm.append(deck.dealcards());
    }
    currentPickingHand = qMakePair(startingPlayer, handToPickForm);
    qDebug() << "dealing cards for turn";
    this->sendGameStateToAll();
    currentState = gameState::PLAYER_TURN;
}

void Game::handlePlayerChoice(Player* player, Card chosenCard)
{
}

void Game::determineRoundWinner() {
    //logic of determine Round Winner;
}

void Game::endGame() {
    //logic of end game;
}

void Game::sendGameStateToAll() {
    QJsonDocument doc = this->buildGameStateDoc(GET_GAME_STATE);
    for (Player* player : gamePlayers) {
        player->getSocket()->write(doc.toJson());
    }
    qDebug() << "sent game state update for all players";
}

QJsonDocument Game::buildGameStateDoc(SERVER_CODES code) {
    QJsonObject message;
    message["type"] = code;
    QJsonArray payloadArray;

    if (currentPickingHand.first !=nullptr) {
        QJsonObject entry;
        QJsonObject playerObj;

        playerObj["username"] = this->currentPickingHand.first->getUsername();
        playerObj["turn"] = this->currentPickingHand.first->isMyTurn();
        entry["player"] = playerObj;

        QJsonArray handArray;
        for (auto card:currentPickingHand.second) {
            QJsonObject cardObj;
            cardObj["suit"] = static_cast<int>(card.getSuit());
            cardObj["rank"] = static_cast<int>(card.getRank());
            handArray.append(cardObj);
        }
        entry["hand"] = handArray;
        payloadArray.append(entry);
    }
    message["payload"] = payloadArray;
    return QJsonDocument(message);
}

void Game::handleGetAvailableCards( QTcpSocket* socket) {
    Player* requestingP = nullptr;
    for (auto p : gamePlayers) {
        if (p->getSocket() == socket) {
            requestingP = p;
        }
    }
    if (!requestingP) return;
    if (this->currentPickingHand.first != requestingP || !requestingP->isMyTurn())
    {
        qDebug() << "Player" << requestingP->getUsername() << "requested cards, but it's not their turn.";
        return;
    }
    QVector<Card> handToPickForm = currentPickingHand.second;
    QJsonObject message;
    message["type"] = GET_AVAILABLE_CARDS;
    QJsonArray payloadArray;

    if (currentPickingHand.first !=nullptr) {
        QJsonObject entry;
        QJsonObject playerObj;

        playerObj["username"] = this->currentPickingHand.first->getUsername();
        playerObj["turn"] = this->currentPickingHand.first->isMyTurn();
        entry["player"] = playerObj;

        QJsonArray handArray;
        for (auto card:currentPickingHand.second) {
            QJsonObject cardObj;
            cardObj["suit"] = static_cast<int>(card.getSuit());
            cardObj["rank"] = static_cast<int>(card.getRank());
            handArray.append(cardObj);
        }
        entry["hand"] = handArray;
        payloadArray.append(entry);
    }
    message["payload"] = payloadArray;
    socket->write(QJsonDocument(message).toJson());

}

void Game::handlePlayerChoice(QTcpSocket* socket,const QJsonObject& request ) {
    Player* choosingP = nullptr;
    int countPlayers = 2;
    for (auto p : gamePlayers) {
        if (p->getSocket() == socket) {
            choosingP = p;
        }
    }
    if (!choosingP||currentPickingHand.first != choosingP||choosingP->isMyTurn()) {
        qDebug<<"handle choosing field";
        return;
    }
    QJsonObject cardObj = request["chosen_card"].toObject();
    Card chosenCard(
        static_cast<Suit>(cardObj["suit"].toInt()),
        static_cast<Rank>(cardObj["rank"].toInt())
    );
    QVector<Card> handToPickForm = currentPickingHand.second;
    if (std::find(handToPickForm.begin(), handToPickForm.end(), chosenCard) != handToPickForm.end()) {
        qDebug()<< "this card not in player hand";
        return;
    }
    choosingP->addCard(chosenCard);
    choosingP->setMyTurn(false);

    qDebug()<<choosingP->getUsername()<<"chosen_card";

    QVector<Card> remainingCards = this->currentPickingHand.second;
    remainingCards.removeOne(chosenCard);

    if (remainingCards.count()>7-countPlayers) {
        int nextPlayerIndex = (startingPlayerIndex + 1) % gamePlayers.size();
        Player* nextPlayer = gamePlayers[nextPlayerIndex];
        nextPlayer->setMyTurn(true);

        currentPickingHand = qMakePair(nextPlayer, remainingCards);
        sendGameStateToAll();
    }
    else {
        if (gamePlayers[0]->getHand().size()>=5) {
            currentState = EVALUATING;
            determineRoundWinner();
        }
        else {
            startingPlayerIndex = (startingPlayerIndex + 1) % gamePlayers.size();
            dealCardsForTurn();
        }
    }
}