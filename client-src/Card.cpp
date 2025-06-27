#include "Card.h"

bool Card::compare_value(const Card& other) const
{
    return value > other.value;
}

bool Card::compare_type(const Card& other) const
{
    return type >= other.type;
}
