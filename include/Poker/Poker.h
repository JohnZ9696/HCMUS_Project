#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <map>
#include <array>
#include "PokerPlayerStats.h"

std::string evaluateHandStrength(const Cards cards[5]);

int handRank(const std::string& handStrength);

bool compareHands(const Cards hand1[5], const Cards hand2[5], const std::string& strength1, const std::string& strength2);

std::vector<int> findWinner(const std::vector<Players>& players);

void Poker(std::vector<Players> &players, int playerAmount);


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
    return 1;
}

void updateStrategy(int plr_idx, std::vector<Players> &players) {
    if (players[plr_idx].handStrength == "Straight Flush") players[plr_idx].Straight_flush++;
    else if (players[plr_idx].handStrength == "Four of a Kind") players[plr_idx].Four_of_a_kind++;
    else if (players[plr_idx].handStrength == "Full House") players[plr_idx].Full_house++;
    else if (players[plr_idx].handStrength == "Flush") players[plr_idx].Flush++;
    else if (players[plr_idx].handStrength == "Straight") players[plr_idx].Straight++;
    else if (players[plr_idx].handStrength == "Three of a Kind") players[plr_idx].Three_of_a_kind++;
    else if (players[plr_idx].handStrength == "Two Pair") players[plr_idx].Two_pair++;
    else if (players[plr_idx].handStrength == "One Pair") players[plr_idx].One_pair++;
    else if (players[plr_idx].handStrength == "High Card") players[plr_idx].High_card++;
}

void findFavoriteStrategy(std::vector<Players> &players) {
    for (auto& player : players) {
    // Create a vector of pairs to represent strategy types and their priorities
    std::vector<std::pair<int, std::string>> strategies = {
        {player.Straight_flush, "Straight Flush"},
        {player.Four_of_a_kind, "Four of a Kind"},
        {player.Full_house, "Full House"},
        {player.Flush, "Flush"}, 
        {player.Straight, "Straight"},
        {player.Three_of_a_kind, "Three of a Kind"},
        {player.Two_pair, "Two Pair"},
        {player.One_pair, "One Pair"},
        {player.High_card, "High Card"}
    };

    // Find the strategy with the highest priority (ties resolved by order)
    auto best_strategy = std::max_element(
        strategies.begin(),
        strategies.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first; // Compare values
        }
    );

    // If no valid strategy (all zero), skip this player
    if (best_strategy->first == 0) continue;

    // Assign the favorite strategy
    player.favorite_strategy = best_strategy->second;
    }
}

bool compareHands(const Cards hand1[5], const Cards hand2[5], const std::string& strength1, const std::string& strength2) {
      int rank1 = handRank(strength1);
    int rank2 = handRank(strength2);
    if (rank1 != rank2) return rank1 > rank2;

    // Tie-breaking logic for specific hand strengths
    std::array<int, 5> ranks1, ranks2;
    for (int i = 0; i < 5; ++i) {
        ranks1[i] = hand1[i].rank;
        ranks2[i] = hand2[i].rank;
    }

    // Sort ranks in descending order
    std::sort(ranks1.rbegin(), ranks1.rend());
    std::sort(ranks2.rbegin(), ranks2.rend());

    if (strength1 == "Straight Flush" || strength1 == "Straight") {
        // Compare the highest card in the straight
        return ranks1[0] > ranks2[0];
    }

    if (strength1 == "Four of a Kind") {
        // Compare the four cards, then the kicker
        int four1 = -1, four2 = -1, kicker1 = -1, kicker2 = -1;
        for (int r : ranks1) {
            if (std::count(ranks1.begin(), ranks1.end(), r) == 4) four1 = r;
            else kicker1 = r;
        }
        for (int r : ranks2) {
            if (std::count(ranks2.begin(), ranks2.end(), r) == 4) four2 = r;
            else kicker2 = r;
        }
        if (four1 != four2) return four1 > four2;
        return kicker1 > kicker2;
    }

    if (strength1 == "Full House") {
        // Compare the three cards, then the pair
        int three1 = -1, three2 = -1, pair1 = -1, pair2 = -1;
        for (int r : ranks1) {
            if (std::count(ranks1.begin(), ranks1.end(), r) == 3) three1 = r;
            else pair1 = r;
        }
        for (int r : ranks2) {
            if (std::count(ranks2.begin(), ranks2.end(), r) == 3) three2 = r;
            else pair2 = r;
        }
        if (three1 != three2) return three1 > three2;
        return pair1 > pair2;
    }

    if (strength1 == "Flush") {
        // Compare cards one by one in descending order
        for (int i = 0; i < 5; ++i) {
            if (ranks1[i] != ranks2[i]) return ranks1[i] > ranks2[i];
        }
    }

    if (strength1 == "Three of a Kind") {
        // Compare the three cards, then the kickers
        int three1 = -1, three2 = -1;
        std::vector<int> kickers1, kickers2;
        for (int r : ranks1) {
            if (std::count(ranks1.begin(), ranks1.end(), r) == 3) three1 = r;
            else kickers1.push_back(r);
        }
        for (int r : ranks2) {
            if (std::count(ranks2.begin(), ranks2.end(), r) == 3) three2 = r;
            else kickers2.push_back(r);
        }
        if (three1 != three2) return three1 > three2;
        std::sort(kickers1.rbegin(), kickers1.rend());
        std::sort(kickers2.rbegin(), kickers2.rend());
        for (size_t i = 0; i < kickers1.size(); ++i) {
            if (kickers1[i] != kickers2[i]) return kickers1[i] > kickers2[i];
        }
    }

    if (strength1 == "Two Pair") {
        // Compare the pairs, then the kicker
        std::vector<int> pairs1, pairs2;
        int kicker1 = -1, kicker2 = -1;
        for (int r : ranks1) {
            if (std::count(ranks1.begin(), ranks1.end(), r) == 2) pairs1.push_back(r);
            else kicker1 = r;
        }
        for (int r : ranks2) {
            if (std::count(ranks2.begin(), ranks2.end(), r) == 2) pairs2.push_back(r);
            else kicker2 = r;
        }
        std::sort(pairs1.rbegin(), pairs1.rend());
        std::sort(pairs2.rbegin(), pairs2.rend());
        for (size_t i = 0; i < pairs1.size(); ++i) {
            if (pairs1[i] != pairs2[i]) return pairs1[i] > pairs2[i];
        }
        return kicker1 > kicker2;
    }

    if (strength1 == "One Pair") {
        // Compare the pair, then the kickers
        int pair1 = -1, pair2 = -1;
        std::vector<int> kickers1, kickers2;
        for (int r : ranks1) {
            if (std::count(ranks1.begin(), ranks1.end(), r) == 2) pair1 = r;
            else kickers1.push_back(r);
        }
        for (int r : ranks2) {
            if (std::count(ranks2.begin(), ranks2.end(), r) == 2) pair2 = r;
            else kickers2.push_back(r);
        }
        if (pair1 != pair2) return pair1 > pair2;
        std::sort(kickers1.rbegin(), kickers1.rend());
        std::sort(kickers2.rbegin(), kickers2.rend());
        for (size_t i = 0; i < kickers1.size(); ++i) {
            if (kickers1[i] != kickers2[i]) return kickers1[i] > kickers2[i];
        }
    }

    if (strength1 == "High Card") {
        // Compare cards one by one in descending order
        for (int i = 0; i < 5; ++i) {
            if (ranks1[i] != ranks2[i]) return ranks1[i] > ranks2[i];
        }
    }

    // If everything is the same, return false (tie)
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