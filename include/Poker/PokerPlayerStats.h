#ifndef PLAYERS_H
#define PLAYERS_H

#include <string>
#include "Poker/PokerCards.h"

struct Players {
    std::string username = "";
    float winrate = 0.0;
    float total_winrate = 0.0;
    std::string favorite_strategy;
    int wins = 0;
    int battles = 0;
    int total_wins = 0;
    int total_battles = 0;
    Cards cards[5];
    short card_id[5] = {-1, -1, -1, -1, -1};
    bool openingCards[5] = {0};
    std::string handStrength;
};

#endif // PLAYERS_H
