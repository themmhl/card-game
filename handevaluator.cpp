#include "handevaluator.h"
#include <algorithm>

HandEvaluator::HandEvaluator(){}

HandRankType HandEvaluator::evaluateHand(QVector<Card> hand){
    std::sort(hand.begin(),hand.end(),[](const Card &a,const Card &b){return a.getRank()<b.getRank();});
    Hand = hand;
    if(isMESSY_HAND()) return HandRankType::MESSY_HAND;
    if(isSINGLE_PAIR()) return HandRankType::SINGLE_PAIR;
    if(isDOUBLE_PAIR()) return HandRankType::DOUBLE_PAIR;
    if(isTHREE_OF_A_KIND()) return HandRankType::THREE_OF_A_KIND;
    if(isSERIES()) return HandRankType::SERIES;
    if(isMSC_HAND()) return HandRankType::MSC_HAND;
    if(isPENTHOUSE_HAND()) return HandRankType::PENTHOUSE_HAND;
    if(isFOUR_OF_A_KIND()) return HandRankType::FOUR_OF_A_KIND;
    if(isORDER_HAND()) return HandRankType::ORDER_HAND;
    if(isGOLDEN_HAND()) return HandRankType::GOLDEN_HAND;
    return HandRankType::NO_RANK;
}
