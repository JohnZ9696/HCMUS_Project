#include "Baccarat/BaccaratPlayerStats.h"
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>


int calculateScore(const Baccarat_Cards cards[], int cardCount) {
    int score = 0;
    for (int i = 0; i < cardCount; ++i) {
        int value = (cards[i].rank >= Baccarat_Cards::TEN) ? 0 : cards[i].rank;
        score += value;
    }
    return score % 10; // Only keep the last digit
}


// Determine the winner(s)
void determineWinner(const std::vector<Baccarat_Players>& players, std::vector<int>& winners) {
    int highestScore = -1;
    int idx = 0;
    for (const auto& player : players) {
        if (player.score >= highestScore) {
            highestScore = player.score;
            winners.push_back(idx);
        }
        idx++;
    }

    std::cout << "Winner(s): ";
    for (const auto& winner : winners) {
        std::cout << winner << " ";
    }
    std::cout << "with a score of " << highestScore << "!" << std::endl;
}

void clearAllBaccaratPlayerName(std::vector<Baccarat_Players> players, int playerAmount) {
    for (int i = 0; i < playerAmount; i++) {
        players[i].username = "";
    }
}
bool BaccaratAlreadyUsedName (std::vector<Baccarat_Players> &players, std::string name, short playerAmount, short plrOrder) {
    if (playerAmount == 0) return false;
    for (int i = 0; i < playerAmount; i++) {
        if (i != plrOrder && players[i].username == name) return true;
    }
    return false;
}
void Baccarat(std::vector<Baccarat_Players> &players, int playerAmount) {
    // Initialize random number generator
    int mark[52] = {0};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 51);
    // Assign random cards to players
    for (int i = 0; i < playerAmount; i++) {
        for (int j = 0; j < 3; j++) {
            int random_number;
            do {
                random_number = dist(gen);
            } while (mark[random_number] != 0);

            mark[random_number] = 1; // Mark card as used
            players[i].card_id[j] = random_number;

            // Map card_id to rank and suit
            int rankIndex = random_number / 4;
            int suitIndex = random_number % 4;
            players[i].cards[j].rank = static_cast<Baccarat_Cards::Rank>(rankIndex);
            players[i].cards[j].suit = static_cast<Baccarat_Cards::Suit>(suitIndex);
        }
    }

    for (auto& player : players) {
        calculateScore(player.cards, 3);
    }
    // Print cards for debugging
    // for (const auto& player : players) {
    //     std::cout << "Player: " << player.username << "\n";
    //     for (const auto& card : player.cards) {
    //         std::cout << "  " << card.rank << " of " << card.suit << "\n";
    //     }
    // }

}