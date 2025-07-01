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

enum gameState{PENDING,ROUND_STARTING,DEALING_CARDS,PLAYER_TURN,EVALUATING,ROUND_OVER,GAME_OVER2};
class Game : public QObject {
    Q_OBJECT
    void startNewRound();
    QVector<Card> dealCardsForTurn();
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
    gameState currentState;
    QVector<Card> initialCards;
    QPair<Player*, QVector<Card>> currentPickingHand;
    Player* pauser;
    QTimer* pauseTimer;
    QMap<Player*, int> stopRequestCount;
signals:
    void gameOver();
private slots:
    void onPauseTimeout();
public:
    explicit Game(const QList<Player*>& players, QObject *parent = nullptr);
    void startGame();
    QList<Player*> getPlayers() const;
    void handleorderPlayers();
    void handleGetAvailableCards(QTcpSocket* socket);
    void handlePlayerChoice(QTcpSocket* socket,const QJsonObject& request);


};

#endif //GAME_H
