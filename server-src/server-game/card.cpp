#include "card.h"

Card::Card() {}
Card::Card(Suit suit, Rank rank){
  cardSuit = suit;
  cardRank = rank;
}
Suit Card::getSuit() const
{
    return cardSuit;
}

Rank Card::getRank() const
{
    return cardRank;
}

bool operator==(const Card& lhs, const Card& rhs) {
    return lhs.getRank() == rhs.getRank() && lhs.getSuit() == rhs.getSuit();
}

