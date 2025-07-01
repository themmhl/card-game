#include "game.h"
#include <QJsonObject>
#include <QJsonDocument>

Game::Game(const QList<Player*>& players, QObject *parent ) :QObject(parent){
    gamePlayers = players;
    currentRound = 0;
    startingPlayerIndex = 0;
    pauseTimer = new QTimer(this);
    connect(pauseTimer, SIGNAL(timeout()), this, SLOT(onPauseTimeout()));
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
    QMap<Player*,Card> initialCards;
    Player* ruler;
    bool isRulerIdentified =false;
    while (!isRulerIdentified) {
        for (Player* player : gamePlayers) {
            initialCards.insert(player, deck.dealcards());
        }

        std::sort(initialCards.begin(), initialCards.end(),[](const Card& card1, const Card& card2) {
            card1.getRank()>=card2.getRank();
        });
        if (initialCards[gamePlayers[0]].getRank() == initialCards[gamePlayers[1]].getRank()) {
            isRulerIdentified = false;
        }
        else {
            for (auto player : gamePlayers) {
                QJsonObject totalPlayer;
                int numPlayer = 0;
                for (auto it = initialCards.begin(); it != initialCards.end(); it++) {
                    QJsonObject playerEntry;
                    playerEntry["username"] = it.key()->getUsername();
                    QJsonObject cardObj;
                    cardObj["suit"] = static_cast<int>(it.value().getSuit());
                    cardObj["rank"] = static_cast<int>(it.value().getRank());
                    playerEntry["initial_card"] = cardObj;
                    totalPlayer[std::to_wstring(numPlayer++)] = playerEntry;
                }
                sendJsonReact(player->getSocket(),GET_INITIAL_CARDS,totalPlayer);
            }
            isRulerIdentified = true;
        }
    }
    qDebug() << "startNewRound:: The ruler was determined";
    startingPlayerIndex = 0;
    gamePlayers.clear();
    for (auto it = initialCards.constBegin(); it != initialCards.constEnd(); ++it) {
        gamePlayers.append(it.key());
    }
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
    handleGetAvailableCards(startingPlayer->getSocket());
    currentState = PLAYER_TURN;
}



void Game::determineRoundWinner() {
    qDebug() << "--- Round" << this->currentRound << "Finished: Evaluating Hands ---";
    this->currentState = EVALUATING;
    QMap<Player*, HandRankType> playerHandRanks;
    HandEvaluator handEvaluator;
    handEvaluator.determineRoundWinner(gamePlayers);
    //creat json object
    QJsonObject roundResultPayload;
    roundResultPayload["winner_username"] = gamePlayers[0]->getUsername();
    roundResultPayload["winning_hand_type"] = static_cast<int>(handEvaluator.evaluateHand(gamePlayers[0]->getHand()));

    QJsonArray allHandsArray;
    for (Player* p : gamePlayers)
    {
        QJsonObject playerHandObj;
        playerHandObj["username"] = p->getUsername();
        QJsonArray handArray;
        for (const Card& card : p->getHand())
        {
            QJsonObject cardObj;
            cardObj["suit"] = static_cast<int>(card.getSuit());
            cardObj["rank"] = static_cast<int>(card.getRank());
            handArray.append(cardObj);
        }
        playerHandObj["hand"] = handArray;
        allHandsArray.append(playerHandObj);
    }
    roundResultPayload["all_players_hands"] = allHandsArray;
    QJsonObject scoresObj;
    for (Player* p : gamePlayers)
    {
        scoresObj[p->getUsername()] = p->getScore();
    }
    roundResultPayload["current_scores"] = scoresObj;

    for (Player* p : gamePlayers)
    {
        sendJsonReact(p->getSocket(),ROUND_RESULT,roundResultPayload);

    }

    qDebug() << "Sent ROUND_RESULT to all players.";
    _sleep(3000);
    for (auto p:gamePlayers) {
        if (p->getScore() >= 2)
        {
            this->endGame();
        }
        else
        {
            this->startNewRound();
        }
    }



}

void Game::endGame() {
    currentState = gameState::GAME_OVER2;
    qDebug() << "game over";
    Player* finalWinner = nullptr;
    for(Player* p : this->gamePlayers)
    {
        if (p->getScore() >= 2)
        {
            finalWinner  = p;
            break;
        }
    }
    if (finalWinner != nullptr) {
        qDebug()<< "endGame::No winner found inside. ";
        return;
    }
    qDebug() << "endGame::Game over";
    QJsonObject gameOverPayload;
    gameOverPayload["final_winner_username"] = finalWinner->getUsername();
    QJsonObject allPlayersScore;
    for (Player* p : this->gamePlayers) {
        allPlayersScore[p->getUsername()] = p->getScore();
    }
    gameOverPayload["final_score"] = allPlayersScore;
    for (Player *p : this->gamePlayers) {
        sendJsonReact(p->getSocket(),SERVER_CODES::GAME_OVER,gameOverPayload);
    }
    emit gameOver();
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

        playerObj["username"] = currentPickingHand.first->getUsername();
        playerObj["turn"] = currentPickingHand.first->isMyTurn();
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
    if (!choosingP||currentPickingHand.first != choosingP||!choosingP->isMyTurn()) {
        qDebug<<"handlePlayerChoice::handle choosing field";
        return;
    }
    QJsonObject cardObj = request["chosen_card"].toObject();
    Card chosenCard(
        static_cast<Suit>(cardObj["suit"].toInt()),
        static_cast<Rank>(cardObj["rank"].toInt())
    );
    QVector<Card> handToPickForm = currentPickingHand.second;
    if (std::find(handToPickForm.begin(), handToPickForm.end(), chosenCard) == handToPickForm.end()) {
        qDebug()<< "handlePlayerChoice::this card not in player hand";
        return;
    }
    choosingP->addCard(chosenCard);
    choosingP->setMyTurn(false);

    qDebug()<<"handlePlayerChoice::"<<choosingP->getUsername()<<"chosen_card";

    QVector<Card> remainingCards = currentPickingHand.second;
    remainingCards.removeOne(chosenCard);

    if (remainingCards.count()>7-countPlayers) {
        int nextPlayerIndex = (startingPlayerIndex + 1) % gamePlayers.size();
        Player* nextPlayer = gamePlayers[nextPlayerIndex];
        nextPlayer->setMyTurn(true);
        currentPickingHand = qMakePair(nextPlayer, remainingCards);
        handleGetAvailableCards(nextPlayer->getSocket());
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

void Game::sendJsonReact(QTcpSocket *socket, SERVER_CODES request, QJsonObject message)
{
    QJsonObject responseObject;

    responseObject["type"] = request;
    responseObject["payload"] = message;

    QJsonDocument responseDoc(responseObject);
        socket->write(responseDoc.toJson());
}
