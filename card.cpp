#include "card.h"

Card::Card() {}

Suit Card::getSuit() const
{
    return cardSuit;
}

Rank Card::getRank() const
{
    return cardRank;
}

bool Card::isValid() const
{
    if(cardRank == Rank::INVALID_RANK &&cardSuit == Suit::INVALID_SUIT)
        return false;
    return true;
}

