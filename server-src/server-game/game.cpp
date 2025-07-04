#include "game.h"
#include <QJsonObject>
#include <QJsonDocument>

void Game::onTurnTimeout()
{
    Player* currentPlayer = currentPickingHand.first;
    if (!currentPlayer) return;

    if (isExtraTime)
    {
        turnTimer->stop();
        qDebug() << "onTurnTimout::Player" << currentPlayer->getUsername() << "timed out completely.";
        if (currentPickingHand.first->minusNotChoosingCardCount())
        {
            qDebug() << "onTurnTimout::Player" << currentPlayer->getUsername() <<
                "has opportunity at minus choosing cards.";
            Card randomCard = currentPickingHand.second.first();
            QVector<Card> remainingCards = currentPickingHand.second;
            remainingCards.removeOne(randomCard);

            if (remainingCards.count() > 7 - gamePlayers.size())
            {
                int nextPlayerIndex = (startingPlayerIndex + 1) % gamePlayers.size();
                Player* nextPlayer = gamePlayers[nextPlayerIndex];
                nextPlayer->setMyTurn(true);
                currentPickingHand = qMakePair(nextPlayer, remainingCards);
                //handleGetAvailableCards(nextPlayer->getSocket());
            }
            else
            {
                if (gamePlayers[0]->getHand().size() >= 5)
                {
                    determineRoundWinner();
                }
                else
                {
                    // startingPlayerIndex = (startingPlayerIndex + 1) % gamePlayers.size();
                    dealCardsForTurn();
                }
            }
        }
        else
        {
        }
    }
    else
    {
        isExtraTime = true;
        turnTimer->start(10000);
        qDebug() << "onTurnTimout::Player" << currentPlayer->getUsername() << "is on 10s extra time.";

        QJsonObject payload;
        payload["time_left"] = 10;
        sendJsonReact(currentPlayer->getSocket(), EXTRA_TIME_STARTED, payload);
    }
}

void Game::onPauseTimeout()
{
}

Game::Game(const QList<Player*>& players, QObject* parent) : QObject(parent)
{
    gamePlayers = players;
    currentRound = 0;
    startingPlayerIndex = 0;
    isExtraTime = false;
    pauseTimer = new QTimer(this);
    turnTimer = new QTimer(this);
    connect(pauseTimer, SIGNAL(timeout()), this, SLOT(onPauseTimeout()));
    chatThread = new QThread(this);
    chatHandler = new ChatHandler();
    chatHandler->moveToThread(chatThread);
    connect(this, &Game::forwardToChatHandler, chatHandler, &ChatHandler::processMessage);
    connect(chatHandler, &ChatHandler::broadcastMessage, this, &Game::broadcastChatMessage);
    connect(chatThread, &QThread::finished, chatHandler, &QObject::deleteLater);
    chatThread->start();
    qDebug() << "created game with" << players.count();
}

void Game::startGame()
{
    qDebug() << "game is starting!";
    startNewRound();
}

QList<Player*> Game::getPlayers() const
{
    return gamePlayers;
}

void Game::handleGetPlayers(QTcpSocket* socket)
{
    if (!socket || socket->state() != QAbstractSocket::ConnectedState)
    {
        qWarning() << "Socket not connected, can’t send JSON";
        return;
    }

    QByteArray jsonData = build_cards_doc(GET_PLAYERS).toJson(QJsonDocument::Compact);
    socket->write(jsonData);
    socket->flush();
    qInfo() << "sent player_data: " << jsonData;
}

void Game::startNewRound()
{
    currentRound++;
    qDebug() << "start new round" << currentRound;
    deck = HandleCard();
    deck.shuffle();

    if (!isRulerIdentified)
    {
        QMap<Player*, Card> initialCards;

        for (Player* player : gamePlayers)
        {
            Card c = deck.dealcards();
            c.cardSuit = DIAMOND;
            initialCards.insert(player, c);
        }

        QList<Player*> sortedPlayersThisDeal = gamePlayers;
        std::sort(sortedPlayersThisDeal.begin(), sortedPlayersThisDeal.end(), [&](Player* a, Player* b)
        {
            Card cardA = initialCards[a];
            Card cardB = initialCards[b];
            if (cardA.getRank() == cardB.getRank())
                cardA.cardRank = static_cast<Rank>(cardA.getRank() + 1);
            return cardA.getRank() > cardB.getRank();
        });

        gamePlayers = sortedPlayersThisDeal;
        startingPlayerIndex = 0;
        isRulerIdentified = true;

        QString rulerName = gamePlayers[startingPlayerIndex]->getUsername();

        QJsonArray playerArr;
        for (Player* p : gamePlayers)
        {
            QJsonObject entry;
            entry["username"] = p->getUsername();

            QJsonObject cardObj;
            Card c = initialCards.value(p);
            cardObj["suit"] = c.getSuit();
            cardObj["rank"] = c.getRank();

            entry["initial_card"] = cardObj;
            playerArr.append(entry);
        }

        QJsonObject payload;
        payload["players_initial_cards"] = playerArr;
        payload["username"] = rulerName;

        for (Player* p : gamePlayers)
        {
            sendJsonReact(p->getSocket(), GET_INITIAL_CARDS, payload);
        }
        qDebug() << "startNewRound:: The ruler was determined. its " << gamePlayers[0]->getUsername();
    }
    else
    {
        startingPlayerIndex = 1 - startingPlayerIndex;
    }
    qInfo() << "it's " << gamePlayers[startingPlayerIndex]->getUsername() << "'s turn";
    currentPickingHand.first = gamePlayers[startingPlayerIndex];
    //start the new round here
    dealCardsForTurn();
}

void Game::dealCardsForTurn()
{
    gamePlayers[startingPlayerIndex]->setMyTurn(true);
    gamePlayers[1 - startingPlayerIndex]->setMyTurn(false);
    Player* startingPlayer = gamePlayers[startingPlayerIndex];

    QJsonObject select_card_info;
    select_card_info["type"] = NEW_TURN;
    select_card_info["username"] = startingPlayer->getUsername();
    QJsonDocument doc(select_card_info);

    QByteArray out = doc.toJson(QJsonDocument::Compact) + "\n";

    QThread::msleep(100);
    qDebug() << "dealing cards for turn->NEW_TURN " << out;
    for (Player* p : gamePlayers)
    {
        p->getSocket()->write(out);
    }
    if (currentTurn++ == 0)
    {
        QVector<Card> handToPickForm;
        for (int i = 0; i < 7; i++)
        {
            handToPickForm.append(deck.dealcards());
        }
        currentPickingHand.second = handToPickForm;
        //if (isRulerIdentified)
        //startingPlayerIndex = 1-startingPlayerIndex;
    }
    currentPickingHand.first = gamePlayers[startingPlayerIndex];
    //  currentTurn = ?0:currentTurn;
    if (currentTurn == 2)
    {
        currentTurn = 0;
        //startingPlayerIndex = 1-startingPlayerIndex;
    }
    //handleGetAvailableCards(startingPlayer->getSocket());
}


void Game::determineRoundWinner()
{
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
    //QByteArray out = QJsonDocument(roundResultPayload).toJson(QJsonDocument::Compact) + "\n";
    for (Player* p : gamePlayers)
    {
        sendJsonReact(p->getSocket(), ROUND_RESULT, roundResultPayload);
    }

    qDebug() << "Sent ROUND_RESULT to all players.";
    QThread::msleep(4000);
    for (auto p : gamePlayers)
    {
        p->clearHand();
        if (p->getScore() >= 2)
        {
            this->endGame();
            return;
        }
    }
    this->startNewRound();
}

void Game::endGame()
{
    //add history here

    qDebug() << "game over bro";
    Player* finalWinner = nullptr;
    for (Player* p : this->gamePlayers)
    {
        if (p->getScore() >= 2)
        {
            finalWinner = p;
            break;
        }
    }
    if (finalWinner == nullptr)
    {
        qDebug() << "endGame::No winner found inside wtf. ";
        return;
    }
    qDebug() << "endGame::Game over";
    QJsonObject gameOverPayload;
    gameOverPayload["final_winner_username"] = finalWinner->getUsername();
    QJsonObject allPlayersScore;
    for (Player* p : this->gamePlayers)
    {
        allPlayersScore[p->getUsername()] = p->getScore();
    }
    gameOverPayload["final_score"] = allPlayersScore;
    //QByteArray out = QJsonDocument(gameOverPayload).toJson(QJsonDocument::Compact) + "\n";
    for (Player* p : this->gamePlayers)
    {
        sendJsonReact(p->getSocket(), SERVER_CODES::GAME_OVER, gameOverPayload);
    }
    emit gameOver();
}

void Game::sendGameStateToAll()
{
    QJsonDocument doc = this->buildGameStateDoc(GET_GAME_STATE);
    for (Player* player : gamePlayers)
    {
        player->getSocket()->write(doc.toJson());
    }
    qDebug() << "sent game state update for all players";
}

QJsonDocument Game::buildGameStateDoc(SERVER_CODES code)
{
    QJsonObject message;
    message["type"] = code;
    QJsonArray payloadArray;

    if (currentPickingHand.first != nullptr)
    {
        QJsonObject entry;
        QJsonObject playerObj;

        playerObj["username"] = this->currentPickingHand.first->getUsername();
        playerObj["turn"] = this->currentPickingHand.first->isMyTurn();
        entry["player"] = playerObj;

        QJsonArray handArray;
        for (auto card : currentPickingHand.second)
        {
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

QJsonDocument Game::build_cards_doc(SERVER_CODES code)
{
    QJsonObject message;
    message["type"] = code;

    QJsonArray full_array;

    int maxPlayers = 2;
    for (int i = 0; i < maxPlayers; ++i)
    {
        Player* p = gamePlayers[i];
        QJsonObject playerObj;
        playerObj["username"] = p->getUsername();
        playerObj["turn"] = p->isMyTurn(); 

        QJsonArray handArray;
        QVector<Card> hand = p->getHand();
        for (int j = 0; j < hand.size(); ++j)
        {
            QJsonObject cardObj;
            cardObj["suit"] = static_cast<int>(hand[j].getSuit());
            cardObj["rank"] = static_cast<int>(hand[j].getRank());
            handArray.append(cardObj);
        }

        QJsonObject entry;
        entry["player"] = playerObj;
        entry["hand"] = handArray;

        full_array.append(entry);
    }

    message["payload"] = full_array;
    return QJsonDocument(message);
}

void Game::handleGetAvailableCards(QTcpSocket* socket)
{
    currentPickingHand.first = gamePlayers[startingPlayerIndex];

    QVector<Card> handToPickForm = currentPickingHand.second;
    QJsonObject message;
    message["type"] = GET_AVAILABLE_CARDS;
    QJsonArray payloadArray;

    if (currentPickingHand.first != nullptr)
    {
        QJsonObject entry;
        QJsonObject playerObj;

        playerObj["username"] = gamePlayers[startingPlayerIndex]->getUsername();
        playerObj["turn"] = gamePlayers[startingPlayerIndex]->isMyTurn();
        entry["player"] = playerObj;

        QJsonArray handArray;
        for (auto card : currentPickingHand.second)
        {
            QJsonObject cardObj;
            cardObj["suit"] = static_cast<int>(card.getSuit());
            cardObj["rank"] = static_cast<int>(card.getRank());
            handArray.append(cardObj);
        }
        entry["hand"] = handArray;
        payloadArray.append(entry);
    }
    message["payload"] = payloadArray;
    QByteArray out = QJsonDocument(message).toJson(QJsonDocument::Compact) + "\n";
    socket->write(out);
    qInfo() << "wrote a message from game side and gave available cards: " << message;
    isExtraTime = false;
    //turnTimer->start(20000);
}

void Game::handlePlayerChoice(QTcpSocket* socket, const QJsonObject& request)
{
    Player* choosingP = nullptr;
    int countPlayers = 2;
    for (auto p : gamePlayers)
    {
        if (p->getSocket() == socket)
        {
            choosingP = p;
        }
    }
    if (!choosingP || currentPickingHand.first != choosingP || !choosingP->isMyTurn())
    {
        qDebug() << "handlePlayerChoice::handle choosing failed";
        return;
    }

    Card chosenCard(
        static_cast<Suit>(request["suit"].toInt()),
        static_cast<Rank>(request["rank"].toInt())
    );
    qInfo() << " chose a card, rank: " << chosenCard.getRank() << " , suit: " << chosenCard.getSuit();
    QVector<Card> handToPickForm = currentPickingHand.second;
    if (std::find(handToPickForm.begin(), handToPickForm.end(), chosenCard) == handToPickForm.end())
    {
        qDebug() << "handlePlayerChoice::this card not in player hand";
        return;
    }
    choosingP->addCard(chosenCard);
    choosingP->setMyTurn(false);

    qDebug() << "handlePlayerChoice::" << choosingP->getUsername() << "chosen_card:  rank:" << request["rank"] <<
        " suit:" << request["suit"];
    QJsonObject responseObject;

    responseObject["type"] = 9;
    responseObject["message"] = "SUCCESS";
    qInfo() << "wrote a message for handlePlayerChoice: " << "SUCCESS";
    QJsonDocument responseDoc(responseObject);
    if (socket && socket->isOpen())
    {
        socket->write(responseDoc.toJson(QJsonDocument::Compact) + "\n");
        socket->flush(); // force send
    }
    QVector<Card> remainingCards = currentPickingHand.second;
    qDebug() << "remove one: " << remainingCards.removeOne(chosenCard);
    startingPlayerIndex = 1 - startingPlayerIndex;
    if (remainingCards.count() > 7 - countPlayers)
    {
        qInfo() << "we here : remainingCards.count() > 7 - countPlayers";


        gamePlayers[startingPlayerIndex]->setMyTurn(true);
        gamePlayers[1 - startingPlayerIndex]->setMyTurn(false);
        currentPickingHand = qMakePair(gamePlayers[startingPlayerIndex], remainingCards);
        dealCardsForTurn();
        //handleGetAvailableCards(nextPlayer->getSocket());
    }
    else
    {
        if (gamePlayers[0]->getHand().size() >= 5)
        {
            determineRoundWinner();
        }
        else
        {
            dealCardsForTurn();
        }
    }
    turnTimer->stop();
}

void Game::handlePauseRequest(QTcpSocket* socket)
{
    Player* RequestingP = nullptr;
    for (auto p : gamePlayers)
    {
        if (p->getSocket() == socket)
        {
            RequestingP = p;
        }
    }
    if (RequestingP->minusPauseRequestingCount())
    {
        pauseTimer->start(20000);
        qDebug() << "handlePauseRequest::minusPauseRequestingCount is ture";
        QJsonObject payload;
        payload["type"] = PAUSE_REQUESTING;
        payload["message"] = RequestingP->getUsername();
        QByteArray out = QJsonDocument(payload).toJson(QJsonDocument::Compact) + "\n";

        for (auto p : gamePlayers)
        {
            p->getSocket()->write(out);
            emit PauseTimeout();
        }
    }
    else
    {
        qDebug() << "handlePauseRequest::minusPauseRequestingCount is false";
        QJsonObject payload;
        payload["type"] = PAUSE_REQUESTING;
        payload["message"] = "EXCESSIVE_USE_OF_PAUSE_REQUEST";
        RequestingP->minusPauseRequestingCount();
    }
}

void Game::sendJsonReact(QTcpSocket* socket, SERVER_CODES request, QJsonObject message)
{
    QJsonObject responseObject;

    responseObject["type"] = request;
    responseObject["payload"] = message;
    qInfo() << "wrote a message from game side: " << responseObject;
    QJsonDocument responseDoc(responseObject);
    if (socket && socket->isOpen())
    {
        socket->write(responseDoc.toJson(QJsonDocument::Compact) + "\n");
        socket->flush(); // force send
    }
}

void Game::endGameTimeout(QTcpSocket* socket)
{
    qDebug() << "game over ";
    Player* finalWinner = nullptr;
    for (int i = 0; i < 2; i++)
    {
        if (socket == gamePlayers[i]->getSocket())
        {
            finalWinner = gamePlayers[1 - i];
            break;
        }
    }
    if (finalWinner == nullptr)
    {
        qDebug() << "endGame::No winner found inside wtf. ";
        return;
    }
    qDebug() << "endGame::Game over";
    QJsonObject gameOverPayload;
    gameOverPayload["final_winner_username"] = finalWinner->getUsername();
    QJsonObject allPlayersScore;
    for (Player* p : this->gamePlayers)
    {
        allPlayersScore[p->getUsername()] = p->getScore();
    }
    gameOverPayload["final_score"] = allPlayersScore;
    //QByteArray out = QJsonDocument(gameOverPayload).toJson(QJsonDocument::Compact) + "\n";
    for (Player* p : this->gamePlayers)
    {
        sendJsonReact(p->getSocket(), SERVER_CODES::GAME_OVER, gameOverPayload);
    }
    emit gameOver();
}

void Game::handleChatRequest(QTcpSocket* socket, const QJsonObject& request)
{
    Player* sender = nullptr;
    for(Player* p : gamePlayers)
    {
        if (p->getSocket() == socket)
        {
            sender = p;
            break;
        }
    }

    if(sender)
    {
        qInfo()<<"got a new message";
        emit forwardToChatHandler(sender, request);
    }
}

void Game::broadcastChatMessage(const QJsonObject& message)
{
    qDebug()<<"main thread: sending chat message to all players";

    for (Player* p : gamePlayers)
    {
        sendJsonReact(p->getSocket(), NEW_CHAT, message);
    }
}
void Game::handleFlipRequest(QTcpSocket *socket) {
    Player* requestingPlayer = nullptr;
    for (auto p : gamePlayers) {
        if (p->getSocket() == socket) {
            requestingPlayer = p;
            break;
        }
    }
    qInfo()<<"requestingplayer? "<<(requestingPlayer!=nullptr);
    if (!requestingPlayer) return;

    if (!flipRequester) {
        flipRequester = requestingPlayer;

        Player* targetPlayer = nullptr;
        for (auto p : gamePlayers) {
            if (p != requestingPlayer) {
                targetPlayer = p;
                break;
            }
        }
        QThread::msleep(200);

        qInfo()<<"targetplayer? "<<(targetPlayer!=nullptr);
        if (targetPlayer) {
            qDebug() << requestingPlayer->getUsername() << " requested a flip. Waiting for " << targetPlayer->getUsername();

            QJsonObject payload;
            payload["type"] = FLIP_REQUEST;
            QJsonDocument out = QJsonDocument(payload);
            if (targetPlayer->getSocket()->isOpen())
            {
                targetPlayer->getSocket()->write(out.toJson(QJsonDocument::Compact) + "\n");
                targetPlayer->getSocket()->flush();
            }
            /*if (turnTimer->isActive()) {
                turnTimeRemainingOnFlip = turnTimer->remainingTime();
                turnTimer->stop();
            }
            flipTimer->start(5000);*/
           // qInfo() << "[FLIP_STEP_1] End: Flip timer started for 5s. Waiting for response.";
        }
    }
    else {
       // flipTimer->stop();
        qInfo() << " start: Player" << requestingPlayer->getUsername() << "accepted the request from" << flipRequester->getUsername();

        QJsonObject payload;
        payload["type"] = FLIP_REQUEST_RESULT;
        payload["message"] = "SUCCESS";
        for (Player* players : this->gamePlayers) {
            QJsonDocument out = QJsonDocument(payload);
            flipRequester = nullptr;
            //qInfo() << "State: Flip requester has been reset.";
            if (players->getSocket()->isOpen())
            {
                players->getSocket()->write(out.toJson(QJsonDocument::Compact) + "\n");
                players->getSocket()->flush();
            }
        }
       // flipTimer->start(10000);
    }

}

void Game::handleFlipChoice(QTcpSocket *socket, const QJsonObject &request) {



    Player* initiator = nullptr;
    for (auto p : gamePlayers) {
        if (p->getSocket() == socket) {
            initiator = p;
            break;
        }
    }
    if (initiator == nullptr) return;
    if (isFirstFlipCardChosen == false) {
        qDebug() << "handleFlipChoice::first choosing process";
        firstCardOfFlip = Card (
            static_cast<Suit>(request["suit"].toInt()),
        static_cast<Rank>(request["rank"].toInt()));
        qInfo()<<initiator->getUsername()<<"chose a card, rank: "<<firstCardOfFlip.getRank()<<" , suit: "<<firstCardOfFlip.getSuit();

        Player* player;
        for (auto p : gamePlayers) {
            if (p->getSocket() == socket) {
                player = p;
            }
        }

       // qDebug() << "handleFlipChoice::" << player->getUsername() << "chosen_card:  rank:" << request["rank"] <<
       //     " suit:" << request["suit"];
        isFirstFlipCardChosen = true;


    }else {
       // qDebug() << "handleFlipChoice::second choosing process";
        Card chosenCard2(
        static_cast<Suit>(request["suit"].toInt()),
        static_cast<Rank>(request["rank"].toInt())
    );
        //qInfo()<<" chose a card, rank: "<<chosenCard2.getRank()<<" , suit: "<<chosenCard2.getSuit();
        Player* player;
        for (auto p : gamePlayers) {
            if (p->getSocket() != socket) {
                player = p;
            }
        }
        player->getReferenceHand().removeOne(chosenCard2);
        player->addCard(firstCardOfFlip);
        initiator->addCard(chosenCard2);
        initiator->getReferenceHand().removeOne(firstCardOfFlip);

       // qDebug() << "handleFlipChoice::" << player->getUsername() << "chosen_card:  rank:" << request["rank"] <<
       //     " suit:" << request["suit"];
        QJsonObject responseObject;



        QJsonDocument responseDoc(responseObject);
        for (auto p : gamePlayers) {
            socket = p->getSocket();
            if (socket && socket->isOpen())
            {
                //اینجا میتونی دو تا کار انجام بدی یکی اینکه همه کارت ها رو برای کلاینت ها بفرستی یکی هم گرفتن تک کارت دیگه هر خودت بهتر بلدی
                // initiator playerبازیکن ها هم یکی شون
            }
        }
        //flipTimer->stop();
        //turnTimer->start(turnTimeRemainingOnFlip);
        isFirstFlipCardChosen = false;
        firstCardOfFlip = Card();
        turnTimeRemainingOnFlip = 0;

    }
}
