#ifndef PLAYERS_H
#define PLAYERS_H

#include <string>
#include "Poker/PokerCards.h"


struct Players {
    std::string username = "";
    float winrate = 0.0;
    std::string favorite_strategy = "...";
    int wins = 0;
    Cards cards[5];
    short card_id[5] = {-1, -1, -1, -1, -1};
    bool openingCards[5] = {0};
    std::string handStrength;
    int High_card = 0, One_pair = 0, Two_pair = 0, Three_of_a_kind = 0, Straight = 0, Flush = 0, Full_house = 0, Four_of_a_kind = 0, Straight_flush = 0;
};

#endif // PLAYERS_H
