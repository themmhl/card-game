#ifndef CARD_H
#define CARD_H

enum CardValue
{
    ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, SOLIDER, QUEEN, KING, BITCOIN
};

enum CardType
{
    COIN, DOLLAR, GOLD, DIAMOND
};

class Card
{
public:
    CardType type;
    CardValue value;

    Card(CardType _type, CardValue _value): type(_type), value(_value)
    {
    };
    bool compare_value(const Card& other) const;
    bool compare_type(const Card& other) const;
};


#endif //CARD_H
