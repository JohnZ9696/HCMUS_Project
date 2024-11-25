#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <array>
#include "PokerPlayerStats.h"

//Prototype
std::string evaluateHandStrength(const Cards cards[5]);

int handRank(const std::string& handStrength);

bool compareHands(const Cards hand1[5], const Cards hand2[5], const std::string& strength1, const std::string& strength2);

std::vector<int> findWinner(const std::vector<Players>& players);

void Poker(std::vector<Players> &players, int playerAmount);

//Definition
std::string evaluateHandStrength(const Cards cards[5]) {
    // Sort cards by rank
    std::array<int, 5> ranks;
    std::array<int, 5> suits;
    for (int i = 0; i < 5; i++) {
        ranks[i] = cards[i].rank;
        suits[i] = cards[i].suit;
    }

    // Sort ranks
    std::sort(ranks.begin(), ranks.end());

    // Count ranks and suits
    std::map<int, int> rankCount;
    std::map<int, int> suitCount;
    for (int i = 0; i < 5; i++) {
        rankCount[ranks[i]]++;
        suitCount[suits[i]]++;
    }

    bool isFlush = (suitCount.size() == 1); // All cards have the same suit
    bool isStraight = (ranks[4] - ranks[0] == 4) && (rankCount.size() == 5);

    // Special case: Ace-low straight (A, 2, 3, 4, 5)
    if (rankCount.size() == 5 && ranks[4] == Cards::ACE && ranks[0] == Cards::TWO &&
        ranks[1] == Cards::THREE && ranks[2] == Cards::FOUR && ranks[3] == Cards::FIVE) {
        isStraight = true;
    }

    // Check for combinations
    if (isStraight && isFlush) {
        return "Straight Flush";
    }
    if (rankCount.size() == 2) {
        // Either Four of a Kind or Full House
        for (const auto& [rank, count] : rankCount) {
            if (count == 4) return "Four of a Kind";
        }
        return "Full House";
    }
    if (isFlush) {
        return "Flush";
    }
    if (isStraight) {
        return "Straight";
    }
    if (rankCount.size() == 3) {
        // Either Three of a Kind or Two Pair
        for (const auto& [rank, count] : rankCount) {
            if (count == 3) return "Three of a Kind";
        }
        return "Two Pair";
    }
    if (rankCount.size() == 4) {
        return "One Pair";
    }
    return "High Card";
}

int handRank(const std::string& handStrength) {
    // Assign numerical values to hand strengths
    if (handStrength == "Straight Flush") return 9;
    if (handStrength == "Four of a Kind") return 8;
    if (handStrength == "Full House") return 7;
    if (handStrength == "Flush") return 6;
    if (handStrength == "Straight") return 5;
    if (handStrength == "Three of a Kind") return 4;
    if (handStrength == "Two Pair") return 3;
    if (handStrength == "One Pair") return 2;
    return 1; // High Card
}

bool compareHands(const Cards hand1[5], const Cards hand2[5], const std::string& strength1, const std::string& strength2) {
    // Compare hand ranks first
    int rank1 = handRank(strength1);
    int rank2 = handRank(strength2);
    if (rank1 != rank2) return rank1 > rank2;

    // If hand ranks are the same, apply tie-breaking logic
    std::array<int, 5> ranks1, ranks2;
    for (int i = 0; i < 5; ++i) {
        ranks1[i] = hand1[i].rank;
        ranks2[i] = hand2[i].rank;
    }

    // Sort ranks in descending order
    std::sort(ranks1.rbegin(), ranks1.rend());
    std::sort(ranks2.rbegin(), ranks2.rend());

    for (int i = 0; i < 5; ++i) {
        if (ranks1[i] != ranks2[i]) {
            return ranks1[i] > ranks2[i];
        }
    }

    // If ranks are identical, return false (tie)
    return false;
}

std::vector<int> findWinner(const std::vector<Players>& players) {
    std::vector<int> winners;
    int bestPlayerIndex = -1;
    std::string bestHandStrength;
    const Cards* bestHand = nullptr;

    for (size_t i = 0; i < players.size(); ++i) {
        const auto& player = players[i];
        std::string handStrength = evaluateHandStrength(player.cards);

        if (bestPlayerIndex == -1 || 
            compareHands(player.cards, bestHand, handStrength, bestHandStrength)) {
            // New best player found
            bestPlayerIndex = i;
            bestHandStrength = handStrength;
            bestHand = player.cards;
            winners = {static_cast<int>(i)};
        } else if (handRank(handStrength) == handRank(bestHandStrength) &&
                   !compareHands(bestHand, player.cards, bestHandStrength, handStrength)) {
            // Tie
            winners.push_back(i);
        }
    }

    return winners; // Return indices of the winner(s)
}

void Poker(std::vector<Players> &players, int playerAmount) {
    // Initialize random number generator
    int mark[52] = {0};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 51);
    
    // Assign random cards to players
    for (int i = 0; i < playerAmount; i++) {
        for (int j = 0; j < 5; j++) {
            int random_number;
            do {
                random_number = dist(gen);
            } while (mark[random_number] != 0);

            mark[random_number] = 1; // Mark card as used
            players[i].card_id[j] = random_number;

            // Map card_id to rank and suit
            int rankIndex = random_number / 4;
            int suitIndex = random_number % 4;
            players[i].cards[j].rank = static_cast<Cards::Rank>(rankIndex);
            players[i].cards[j].suit = static_cast<Cards::Suit>(suitIndex);
        }
    }

    // Print cards for debugging
    // for (const auto& player : players) {
    //     std::cout << "Player: " << player.username << "\n";
    //     for (const auto& card : player.cards) {
    //         std::cout << "  " << card.rank << " of " << card.suit << "\n";
    //     }
    // }

}