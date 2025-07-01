#include "handevaluator.h"
#include <algorithm>

HandEvaluator::HandEvaluator(){}

HandRankType HandEvaluator::evaluateHand(QVector<Card> hand){
    std::sort(hand.begin(),hand.end(),[](const Card &a,const Card &b){return a.getRank()>b.getRank();});
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
    return HandRankType::NO_RANK ;
}

bool HandEvaluator::isGOLDEN_HAND() {
    const Suit firstSuit = Hand[0].getSuit();
    for (int i = 1; i < 5; ++i) {
        if (Hand[i].getSuit() != firstSuit) {
            return false;
        }
    }


    if (Hand[0].getRank() == Rank::BITCOIN && Hand[4].getRank() == Rank::TEN)  return true;

    return false;
}

bool HandEvaluator::isORDER_HAND() {

    const Suit firstSuit = Hand[0].getSuit();
    for (int i = 1; i < 5; ++i) {
        if (Hand[i].getSuit() != firstSuit) {
            return false;
        }
    }


    for (int i = 1; i < 5; ++i) {
        if (static_cast<int>(Hand[i].getRank())+1 != static_cast<int>(Hand[i-1].getRank())) {
            return false;
        }
    }

    return true;
}

bool HandEvaluator::isFourOfAKind(const QVector<Card>& sortedHand) {
    // In a sorted hand of 5, a four-of-a-kind can only be in two positions:
    // Pattern 1: [A, A, A, A, B]
    bool pattern1 = sortedHand[0].getRank() == sortedHand[1].getRank() &&
                    sortedHand[1].getRank() == sortedHand[2].getRank() &&
                    sortedHand[2].getRank() == sortedHand[3].getRank();

    // Pattern 2: [A, B, B, B, B]
    bool pattern2 = sortedHand[1].getRank() == sortedHand[2].getRank() &&
                    sortedHand[2].getRank() == sortedHand[3].getRank() &&
                    sortedHand[3].getRank() == sortedHand[4].getRank();

    return pattern1 || pattern2;
}

bool HandEvaluator::isPenthouseHand(const QVector<Card>& sortedHand) {
    // A Penthouse Hand is a "Full House" (3 of one rank, 2 of another).
    // In a sorted hand, it can only be in two patterns:
    // Pattern 1: [A, A, A, B, B]
    bool pattern1 = sortedHand[0].getRank() == sortedHand[1].getRank() &&
                    sortedHand[1].getRank() == sortedHand[2].getRank() &&
                    sortedHand[3].getRank() == sortedHand[4].getRank();

    // Pattern 2: [A, A, B, B, B]
    bool pattern2 = sortedHand[0].getRank() == sortedHand[1].getRank() &&
                    sortedHand[2].getRank() == sortedHand[3].getRank() &&
                    sortedHand[3].getRank() == sortedHand[4].getRank();

    return pattern1 || pattern2;
}

bool HandEvaluator::isMscHand(const QVector<Card>& sortedHand) {
    // An MSC Hand is a "Flush" (5 cards of the same suit, not necessarily in sequence).
    const Suit suit = sortedHand[0].getSuit();
    for (int i = 1; i < 5; ++i) {
        if (sortedHand[i].getSuit() != suit) {
            return false;
        }
    }
    return true;
}

bool HandEvaluator::isSeries(const QVector<Card>& sortedHand) {
    // A Series is a "Straight" (5 cards in sequence, can be different suits).
    // First, ensure all ranks are unique. A pair would disqualify a straight.
    QSet<Rank> uniqueRanks;
    for(const auto& card : sortedHand) {
        uniqueRanks.insert(card.getRank());
    }
    if(uniqueRanks.count() != 5) {
        return false; // Not 5 unique ranks, so cannot be a straight.
    }

    // Now, check if the ranks are sequential.
    for (int i = 1; i < 5; ++i) {
        if (static_cast<int>(sortedHand[i].getRank()) != static_cast<int>(sortedHand[i - 1].getRank()) + 1) {
            return false;
        }
    }
    return true;
}

bool HandEvaluator::isThreeOfAKind(const QVector<Card>& sortedHand) {
    // In a sorted hand, three-of-a-kind can be in three positions:
    // Pattern 1: [A, A, A, B, C]
    if (sortedHand[0].getRank() == sortedHand[1].getRank() && sortedHand[1].getRank() == sortedHand[2].getRank()) return true;
    // Pattern 2: [A, B, B, B, C]
    if (sortedHand[1].getRank() == sortedHand[2].getRank() && sortedHand[2].getRank() == sortedHand[3].getRank()) return true;
    // Pattern 3: [A, B, C, C, C]
    if (sortedHand[2].getRank() == sortedHand[3].getRank() && sortedHand[3].getRank() == sortedHand[4].getRank()) return true;

    return false;
}

bool HandEvaluator::isDoublePair(const QVector<Card>& sortedHand) {
    // In a sorted hand, two pairs can be in three positions:
    // Pattern 1: [A, A, B, B, C]
    if (sortedHand[0].getRank() == sortedHand[1].getRank() && sortedHand[2].getRank() == sortedHand[3].getRank()) return true;
    // Pattern 2: [A, A, B, C, C]
    if (sortedHand[0].getRank() == sortedHand[1].getRank() && sortedHand[3].getRank() == sortedHand[4].getRank()) return true;
    // Pattern 3: [A, B, B, C, C]
    if (sortedHand[1].getRank() == sortedHand[2].getRank() && sortedHand[3].getRank() == sortedHand[4].getRank()) return true;

    return false;
}

bool HandEvaluator::isSinglePair(const QVector<Card>& sortedHand) {
    // We can count the number of pairs. If there is exactly one, return true.
    int pairCount = 0;
    if (sortedHand[0].getRank() == sortedHand[1].getRank()) pairCount++;
    if (sortedHand[1].getRank() == sortedHand[2].getRank()) pairCount++;
    if (sortedHand[2].getRank() == sortedHand[3].getRank()) pairCount++;
    if (sortedHand[3].getRank() == sortedHand[4].getRank()) pairCount++;

    return pairCount == 1;
}
