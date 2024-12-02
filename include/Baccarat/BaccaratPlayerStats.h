#ifndef BACCARAT_PLAYERS_H
#define BACCARAT_PLAYERS_H

#include <string>
#include "Baccarat/BaccaratCards.h"

struct Baccarat_Players {
    std::string username = "";
    float winrate = 0.0;
    int wins = 0;
    Baccarat_Cards cards[3];
    short card_id[3] = {-1, -1, -1};
    bool openingCards[3] = {0};
    std::string handType = "Regular";
    int score = 0;
};

#endif
