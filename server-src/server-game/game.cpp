#include "game.h"
#include <QJsonArray>
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
void Game::startNewRound() {
    currentRound++;
    qDebug() << "start new round"<<currentRound;
    deck = HandleCard();
    deck.shuffle();

    dealCardsForTurn();
}
void Game::dealCardsForTurn() {
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
void Game::processPlayerChoice(Player* player, Card chosenCard)
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

