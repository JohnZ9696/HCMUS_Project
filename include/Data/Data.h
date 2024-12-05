#ifndef PLAYER_DATA
#define PLAYER_DATA
#include <string>
#include "vector"
#include "fstream"
struct Player_Data{
    std::string username;
    int wins = 0;
    int battles = 0;
    int winrate = 0;
};

#endif