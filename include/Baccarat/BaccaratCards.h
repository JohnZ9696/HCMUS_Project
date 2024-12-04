#ifndef BACCARAT_CARDS_H
#define BACCARATCARDS_H

#include <string>

struct Baccarat_Cards {
    enum Suit { CLUBS, DIAMONDS, HEARTS, SPADES } suit;
    enum Rank { ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING } rank;
};

#endif
