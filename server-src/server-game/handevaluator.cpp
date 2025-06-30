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
}

void HandEvaluator::determineRoundWinner(QList<Player*>& players){
	if (players.isEmpty())
    {
        return ;
    }
    QMap<Player*, HandRankType> playerRanks;
    for (Player* p : players)
    {
        playerRanks.insert(p, evaluateHand(p->getHand()));
    }
    for(int i=1;i<players.size();i++){
    	Player* firstP =players[i];
        for(int j=i+1;j<players.size();j++){
       		Player* secondP = players[j];
       		if(compareHands(firstP,secondP)==secondP){
                  players[i] = secondP;
                  players[j] = firstP;
       		}
        }
    }
    return;
}

Player* HandEvaluator::compareHands(Player* player1, Player* player2)
{
    auto hand1 = player1->getHand();
    auto hand2 = player2->getHand();
    std::sort(hand1.begin(), hand1.end(), [](const Card& a, const Card& b){ return a.getRank() > b.getRank(); });
    std::sort(hand2.begin(), hand2.end(), [](const Card& a, const Card& b){ return a.getRank() > b.getRank(); });


    HandRankType rank1 = evaluateHand(hand1);
    HandRankType rank2 = evaluateHand(hand2);
    if (rank1 > rank2) return player1;
    if (rank2 > rank1) return player2;

    qDebug() << "Tie occurred with rank" << static_cast<int>(rank1) << ". Breaking tie...";

    switch (rank1)
    {
        case HandRankType::GOLDEN_HAND:
        case HandRankType::ORDER_HAND:
        case HandRankType::SERIES:
        {
            for (int i = 0; i <=4; ++i)
            {
                if (hand1[i].getRank() > hand2[i].getRank()) return player1;
                if (hand2[i].getRank() > hand1[i].getRank()) return player2;
            }
            if (hand1[0].getSuit() > hand2[0].getSuit()) return player1;
            if (hand2[0].getSuit() > hand1[0].getSuit()) return player2;
            break;
        }
        case HandRankType::FOUR_OF_A_KIND:
        case HandRankType::PENTHOUSE_HAND:
        case HandRankType::THREE_OF_A_KIND:
        {

            Rank setRank1 = hand1[2].getRank();
            Rank setRank2 = hand2[2].getRank();
            if (setRank1 > setRank2) return player1;
            if (setRank2 > setRank1) return player2;
            break;
        }
//double pair still works
        case HandRankType::DOUBLE_PAIR:
        {

            QList<Rank> pairs1, kickers1;

            QList<Rank> pairs2, kickers2;



            if (pairs1[0] > pairs2[0]) return player1;
            if (pairs2[0] > pairs1[0]) return player2;

            if (pairs1[1] > pairs2[1]) return player1;
            if (pairs2[1] > pairs1[1]) return player2;

            if (kickers1[0] > kickers2[0]) return player1;
            if (kickers2[0] > kickers1[0]) return player2;
            break;
        }
//single pair still works
        case HandRankType::SINGLE_PAIR:
        {

            Rank pairRank1, pairRank2;
            QList<Rank> kickers1, kickers2;



            if (pairRank1 > pairRank2) return player1;
            if (pairRank2 > pairRank1) return player2;


            for (int i = 0; i < kickers1.size(); ++i)
            {
                if (kickers1[i] > kickers2[i]) return player1;
                if (kickers2[i] > kickers1[i]) return player2;
            }
            break;
        }
        case HandRankType::MSC_HAND:
            if (hand1[0].getRank() > hand2[0].getRank()) return player1;
            if (hand2[0].getRank() > hand1[0].getRank()) return player2;
            if (hand1[0].getSuit() > hand2[0].getSuit()) return player1;
            if (hand2[0].getSuit() > hand1[0].getSuit()) return player2;
            break;
        case HandRankType::MESSY_HAND:
        {
            for (int i = 0; i <=4; ++i)
            {
                if (hand1[i].getRank() > hand2[i].getRank()) return player1;
                if (hand2[i].getRank() > hand1[i].getRank()) return player2;
            }
            for(int i = 0; i<=4; ++i) {
                if (hand1[0].getSuit() > hand2[0].getSuit()) return player1;
                if (hand2[0].getSuit() > hand1[0].getSuit()) return player2;
            }
            break;
        }
        default:
            break;
    }


    return player1;
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
    int pairCount = 0;
    if (sortedHand[0].getRank() == sortedHand[1].getRank()) pairCount++;
    if (sortedHand[1].getRank() == sortedHand[2].getRank()) pairCount++;
    if (sortedHand[2].getRank() == sortedHand[3].getRank()) pairCount++;
    if (sortedHand[3].getRank() == sortedHand[4].getRank()) pairCount++;

    return pairCount == 1;
}
