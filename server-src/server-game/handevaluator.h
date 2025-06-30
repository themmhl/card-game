#ifndef HANDEVALUATOR_H
#define HANDEVALUATOR_H

#include <QObject>
#include "card.h"
#include "player.h"
#include <QVector>

enum class HandRankType {
    MESSY_HAND,
    SINGLE_PAIR,
    DOUBLE_PAIR,
    THREE_OF_A_KIND,
    SERIES,
    MSC_HAND,
    PENTHOUSE_HAND,
    FOUR_OF_A_KIND,
    ORDER_HAND,
    GOLDEN_HAND
};
class HandEvaluator
{
public:
    HandEvaluator();
    HandRankType evaluateHand(QVector<Card> hand);
    void determineRoundWinner( QList<Player*>& players);
    Player* compareHands(Player* player1, Player* player2);


private:
    QVector<Card> Hand;
    Player* breakTie(Player* palyer1 ,Player* palyer2);
    bool isMESSY_HAND();
    bool isSINGLE_PAIR();
    bool isDOUBLE_PAIR();
    bool isTHREE_OF_A_KIND();
    bool isSERIES();
    bool isMSC_HAND();
    bool isPENTHOUSE_HAND();
    bool isFOUR_OF_A_KIND();
    bool isORDER_HAND();
    bool isGOLDEN_HAND();
};

#endif // HANDEVALUATOR_H
