#ifndef CARD_H
#define CARD_H

#include <QObject>
enum Suit {
    DIAMOND,
    DOLLAR,
    COIN,
    GOLD,
    INVALID_SUIT
};

enum Rank {
    TWO, THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    SOLDIER,
    QUEEN,
    KING,
    BITCOIN,
    INVALID_RANK
};

class Card
{
public:
    Card();
    Card(Suit suit, Rank rank);

    Suit getSuit() const;
    Rank getRank() const;
    bool isValid() const;

    Suit cardSuit;
    Rank cardRank;
};

#endif // CARD_H
