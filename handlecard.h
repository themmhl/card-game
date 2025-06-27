#ifndef HANDLECARD_H
#define HANDLECARD_H
#include "card.h"
#include<QVector>
class HandleCard
{
    QVector<Card> cards;
public:
    HandleCard();
    void shuffle();
    Card dealcards();
    int count() const;
};

#endif // HANDLECARD_H
