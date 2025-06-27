#ifndef HANDEVALUATOR_H
#define HANDEVALUATOR_H

#include <QObject>
#include "card.h"
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
    GOLDEN_HAND,
    NO_RANK
};
class HandEvaluator
{
public:
    HandEvaluator();
    explicit HandEvaluator(QVector<Card> Hand) : Hand(std::move(Hand)) {}
    HandRankType evaluateHand(QVector<Card> hand);

private:
    QVector<Card> Hand;
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
