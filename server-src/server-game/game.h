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

class Game : public QObject {
    Q_OBJECT
    void startNewRound();
    void dealCardsForTurn();
    void handlePlayerChoice(Player *player, Card chosenCard);
    void determineRoundWinner();
    void endGame();
    void sendGameStateToAll();
    void sendJsonReact(QTcpSocket *socket, SERVER_CODES request, QJsonObject message);

    QJsonDocument buildGameStateDoc(SERVER_CODES code);

    QList<Player*> gamePlayers;
    HandleCard deck;
    int currentRound;
    int startingPlayerIndex;

    QVector<Card> initialCards;
    QPair<Player*, QVector<Card>> currentPickingHand;
    Player* pauser;
    bool isExtraTime;
    QTimer* pauseTimer;
    QTimer* turnTimer;
signals:
    void gameOver();

    void PauseTimeout();
private slots:
    void onTurnTimeout();
    void onPauseTimeout();
public:
    explicit Game(const QList<Player*>& players, QObject *parent = nullptr);
    void startGame();
    QList<Player*> getPlayers() const;
    void handleorderPlayers();
    void handleGetAvailableCards(QTcpSocket* socket);
    void handlePlayerChoice(QTcpSocket* socket,const QJsonObject& request);
    void handlePauseRequest(QTcpSocket* socket);


};

#endif //GAME_H
