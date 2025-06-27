#include "handevaluator.h"
#include <algorithm>

HandEvaluator::HandEvaluator(){}

HandRankType HandEvaluator::evaluateHand(QVector<Card> hand){
    std::sort(hand.begin(),hand.end(),[](const Card &a,const Card &b){return a.getRank()<b.getRank();});
    Hand = hand;

}
