#include "handlecard.h"
#include <algorithm>
#include <random>
#include <QDebug>

HandleCard::HandleCard() {
    cards.reserve(52);
    for (int s = 0; s < 4; ++s) {
        for (int r = 0; r < 13; ++r) {
            cards.append(Card(static_cast<Suit>(s), static_cast<Rank>(r)));
        }
    }
}

void HandleCard::shuffle()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(cards.begin(), cards.end(), gen);
}

Card HandleCard::dealcards()
{
    if (cards.isEmpty()) {
        qWarning() << "Cannot deal from an empty deck.";
        return Card();
    }

    return cards.takeLast();
}

int HandleCard::count() const
{
    return cards.count();
}

