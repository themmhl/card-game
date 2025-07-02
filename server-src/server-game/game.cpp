#include "game.h"
#include <QJsonObject>
#include <QJsonDocument>

void Game::onTurnTimeout() {
    Player* currentPlayer = currentPickingHand.first;
    if (!currentPlayer) return;

    if (isExtraTime) {

        turnTimer->stop();
        qDebug() << "onTurnTimout::Player" << currentPlayer->getUsername() << "timed out completely.";
        if (currentPickingHand.first->minusNotChoosingCardCount()) {
            qDebug() << "onTurnTimout::Player" << currentPlayer->getUsername() << "has opportunity at minus choosing cards.";
            Card randomCard = currentPickingHand.second.first();
            QVector<Card> remainingCards = currentPickingHand.second;
            remainingCards.removeOne(randomCard);

            if (remainingCards.count()>7-gamePlayers.size()) {
                int nextPlayerIndex = (startingPlayerIndex + 1) % gamePlayers.size();
                Player* nextPlayer = gamePlayers[nextPlayerIndex];
                nextPlayer->setMyTurn(true);
                currentPickingHand = qMakePair(nextPlayer, remainingCards);
                handleGetAvailableCards(nextPlayer->getSocket());
            }
            else {
                if (gamePlayers[0]->getHand().size()>=5) {

                    determineRoundWinner();
                }
                else {
                    startingPlayerIndex = (startingPlayerIndex + 1) % gamePlayers.size();
                    dealCardsForTurn();
                }
            }
        } else {

        }

    } else {
        isExtraTime = true;
        turnTimer->start(10000);
        qDebug() << "onTurnTimout::Player" << currentPlayer->getUsername() << "is on 10s extra time.";

        QJsonObject payload;
        payload["time_left"] = 10;
        sendJsonReact(currentPlayer->getSocket(), EXTRA_TIME_STARTED, payload);
    }
}

void Game::onPauseTimeout() {
    
}

Game::Game(const QList<Player*>& players, QObject *parent ) :QObject(parent){
    gamePlayers = players;
    currentRound = 0;
    startingPlayerIndex = 0;
    isExtraTime = false;
    pauseTimer = new QTimer(this);
    turnTimer = new QTimer(this);
    connect(pauseTimer, SIGNAL(timeout()), this, SLOT(onPauseTimeout()));
    connect(this->turnTimer, &QTimer::timeout, this, &Game::onTurnTimeout);

    qDebug() << "created game with"<<players.count();
}

void Game::startGame() {
    qDebug() << "game is starting!";
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

        std::sort(gamePlayers.begin(), gamePlayers.end(),[&](Player* a, Player* b) {
            Card cardA = initialCards.value(a);
            Card cardB = initialCards.value(b);
            return cardA.getRank() > cardB.getRank();
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

void Game::dealCardsForTurn() {

    Player* startingPlayer = gamePlayers[startingPlayerIndex];
    startingPlayer->setMyTurn(true);

    QVector<Card> handToPickForm;
    for (int i = 0;i<7;i++) {
        handToPickForm.append(deck.dealcards());
    }
    currentPickingHand = qMakePair(startingPlayer, handToPickForm);
    qDebug() << "dealing cards for turn";
    handleGetAvailableCards(startingPlayer->getSocket());

}



void Game::determineRoundWinner() {
    qDebug() << "--- Round" << this->currentRound << "Finished: Evaluating Hands ---";

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
    QThread::msleep(3000);
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

    isExtraTime = false;
    turnTimer->start(20000);

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
        qDebug()<<"handlePlayerChoice::handle choosing field";
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

            determineRoundWinner();
        }
        else {
            startingPlayerIndex = (startingPlayerIndex + 1) % gamePlayers.size();
            dealCardsForTurn();
        }
    }
    turnTimer->stop();
}

void Game::handlePauseRequest(QTcpSocket *socket) {
    Player* RequestingP = nullptr;
    for (auto p : gamePlayers) {
        if (p->getSocket() == socket) {
            RequestingP = p;
        }
    }
    if (RequestingP->minusPauseRequestingCount()) {
        pauseTimer->start(20000);
        qDebug()<<"handlePauseRequest::minusPauseRequestingCount is ture";
        QJsonObject payload;
        payload["type"] = PAUSE_REQUESTING;
        payload["message"] = RequestingP->getUsername();
        for (auto p : gamePlayers) {
            p->getSocket()->write(QJsonDocument(payload).toJson());
            emit PauseTimeout();
        }
    }
    else {
        qDebug()<<"handlePauseRequest::minusPauseRequestingCount is false";
        QJsonObject payload;
        payload["type"] = PAUSE_REQUESTING;
        payload["message"] = "EXCESSIVE_USE_OF_PAUSE_REQUEST";
        RequestingP->minusPauseRequestingCount();
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

