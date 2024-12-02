#ifndef BACCARAT_CARDS_H
#define BACCARATCARDS_H

#include <string>

struct Baccarat_Cards {
    enum Suit { CLUBS, DIAMONDS, HEARTS, SPADES } suit;
    enum Rank { TWO = 2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE } rank;
};

#endif
