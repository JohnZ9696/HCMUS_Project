#ifndef CARDS_H
#define CARDS_H

#include <string>

struct Cards {
    enum Suit { SPADES, CLUBS, DIAMONDS, HEARTS } suit;
    enum Rank { TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE } rank;
};

#endif // CARDS_H
