#ifndef GAME_H
#define GAME_H
#include <QObject>
#include <QList>
#include <QJsonArray>
#include "player.h"
#include "handevaluator.h"
#include "handlecard.h"
#include "sharedDefs.h"
#include <QTimer>
#include <QThread>

#include "chathandler.h"

class Game : public QObject
{
    Q_OBJECT
    void startNewRound();
    void dealCardsForTurn();
    void determineRoundWinner();
    void endGame();
    void sendGameStateToAll();
    void sendJsonReact(QTcpSocket* socket, SERVER_CODES request, QJsonObject message);

    QJsonDocument buildGameStateDoc(SERVER_CODES code);
    QJsonDocument build_cards_doc(SERVER_CODES code);
    QList<Player*> gamePlayers;
    HandleCard deck;
    int currentRound = 0;
    int currentTurn = 0;
    int startingPlayerIndex = 0;
    bool isRulerIdentified = false;
    QVector<Card> initialCards;
    QPair<Player*, QVector<Card>> currentPickingHand;
    Player* pauser;
    bool isExtraTime;
    QTimer* pauseTimer;
    QTimer* turnTimer;
    QThread* chatThread;
    ChatHandler* chatHandler;
    QTimer* flipTimer;
    int turnTimeRemainingOnFlip;
    Player* initiator;
    Card firstCardOfFlip;
    bool isFirstFlipCardChosen;
    Player* flipRequester;
signals:
    void gameOver();
    void forwardToChatHandler(Player* sender, const QJsonObject& messageContent);
    void PauseTimeout();

private slots:
    void onTurnTimeout();
    void onPauseTimeout();
    void broadcastChatMessage(const QJsonObject& message);
    //void onFlipTimeout();

public:
    explicit Game(const QList<Player*>& players, QObject* parent = nullptr);
    void startGame();
    QList<Player*> getPlayers() const;
    void handleGetPlayers(QTcpSocket* socket);
    void handleGetAvailableCards(QTcpSocket* socket);
    void handlePlayerChoice(QTcpSocket* socket, const QJsonObject& request);
    void handlePauseRequest(QTcpSocket* socket);
    void endGameTimeout(QTcpSocket* socket);
    void handleChatRequest(QTcpSocket* socket, const QJsonObject& request);
    void handleFlipRequest(QTcpSocket* socket);
    void handleFlipChoice(QTcpSocket* socket,const QJsonObject& request);
};

#endif //GAME_H