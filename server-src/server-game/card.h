#ifndef CARD_H
#define CARD_H

#include <QObject>
enum Suit {
    DIAMOND,
    DOLLAR,
    COIN,
    GOLD,
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
    BITCOIN
};

class Card
{
public:
    Card();
    Card(Suit suit, Rank rank);

    Suit getSuit() const;
    Rank getRank() const;
    friend bool operator==(const Card& lhs, const Card& rhs) ;
    Suit cardSuit;
    Rank cardRank;

};


#endif // CARD_H
