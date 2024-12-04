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

void renderPokerResultBoard(SDL_Renderer* renderer, TTF_Font* font, int scrollOffset, std::vector<Players> players, int playerAmount, int cardAmount, SDL_Texture* cardTextures[], std::vector<int> winners_idx) {
    int lineHeight = 125;   

    // Clip content to the frame's area
    SDL_Rect viewport = { 70, 140, 1090, 470 };
    SDL_RenderSetClipRect(renderer, &viewport);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 0);
    SDL_RenderFillRect(renderer, &viewport);

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color winnerColor = { 255, 255, 0, 255 };

    // Use a set for faster winner checks
    std::unordered_set<int> winners(winners_idx.begin(), winners_idx.end());

    SDL_Rect textRect, cardRect, handStrengthRect;
    for (int i = 0; i < playerAmount; ++i) {
        int y = 170 + i * lineHeight - scrollOffset;

        // Skip players outside the viewport
        if (y + lineHeight < viewport.y || y > viewport.y + viewport.h) {
            continue;
        }

        bool winner = winners.find(i) != winners.end();
        SDL_Color currentColor = winner ? winnerColor : textColor;

        // Render player name
        std::string text = players[i].username + ':';
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), currentColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        textRect = { 80, y, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        // Render player cards
        for (int j = 0; j < cardAmount; ++j) {
            cardRect = { textRect.x + textSurface->w + 10 + j * 90, y - 30, 80, 105 };
            SDL_RenderCopy(renderer, cardTextures[players[i].card_id[j]], nullptr, &cardRect);
        }

        // Render hand strength
        std::string handStrength = '(' + players[i].handStrength + ')';
        SDL_Surface* handStrengthSurface = TTF_RenderText_Solid(font, handStrength.c_str(), currentColor);
        SDL_Texture* handStrengthTexture = SDL_CreateTextureFromSurface(renderer, handStrengthSurface);
        handStrengthRect = { cardRect.x + 90, y, handStrengthSurface->w - static_cast<int>(handStrength.length()*3+text.length()*3-2)*2+30, handStrengthSurface->h};
        SDL_RenderCopy(renderer, handStrengthTexture, nullptr, &handStrengthRect);

        // Free resources
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(handStrengthSurface);
        SDL_DestroyTexture(handStrengthTexture);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}

bool Poker_cmp(const Players& a, const Players& b) {
    return a.wins > b.wins;
}

void renderLeaderBoardScroll(SDL_Renderer* renderer, TTF_Font* font, int scrollOffset, std::vector<Players> players, int playerAmount, std::vector<int> winners_idx, SDL_Texture* top1Crown) {
    int lineHeight = 70;

    // Size cua scroll frame
    SDL_Rect viewport = { 70, 200, 1100, 400 };
    SDL_RenderSetClipRect(renderer, &viewport);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 0);
    SDL_RenderFillRect(renderer, &viewport);

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color winnerColor = { 255, 255, 0, 255 };

    std::unordered_set<int> winners(winners_idx.begin(), winners_idx.end());

    SDL_Rect textRect, crownRect;
    for (int i = 0; i < playerAmount; ++i) {
        int y = 200 + i * lineHeight - scrollOffset;

        if (y + lineHeight < viewport.y || y > viewport.y + viewport.h) {
            continue;
        }

        bool isWinner = winners.find(i) != winners.end();
        SDL_Color currentColor = isWinner ? winnerColor : textColor;

        // Render rank
        std::string rank = std::to_string(i + 1);
        if (players[i].wins < players[0].wins) {
            SDL_Surface* rankSurface = TTF_RenderText_Solid(font, rank.c_str(), currentColor);
            SDL_Texture* rankTexture = SDL_CreateTextureFromSurface(renderer, rankSurface);
            textRect = { 120 - static_cast<int>(rank.size()) * 15, y, rankSurface->w, rankSurface->h };
            SDL_RenderCopy(renderer, rankTexture, nullptr, &textRect);
            SDL_FreeSurface(rankSurface);
            SDL_DestroyTexture(rankTexture);
        } else {
            // Render crown cho winner
            crownRect = { 85, y - 5, 65, 50 };
            SDL_RenderCopy(renderer, top1Crown, nullptr, &crownRect);
        }

        // Render username
        std::string username = players[i].username;
        SDL_Surface* usernameSurface = TTF_RenderText_Solid(font, username.c_str(), currentColor);
        SDL_Texture* usernameTexture = SDL_CreateTextureFromSurface(renderer, usernameSurface);
        textRect = { 330 - static_cast<int>(username.size()) * 15, y, usernameSurface->w, usernameSurface->h };
        SDL_RenderCopy(renderer, usernameTexture, nullptr, &textRect);
        SDL_FreeSurface(usernameSurface);
        SDL_DestroyTexture(usernameTexture);

        // Render wins
        std::string wins = std::to_string(players[i].wins);
        SDL_Surface* winsSurface = TTF_RenderText_Solid(font, wins.c_str(), currentColor);
        SDL_Texture* winsTexture = SDL_CreateTextureFromSurface(renderer, winsSurface);
        textRect = { 550 - static_cast<int>(wins.size()) * 19, y, winsSurface->w, winsSurface->h };
        SDL_RenderCopy(renderer, winsTexture, nullptr, &textRect);
        SDL_FreeSurface(winsSurface);
        SDL_DestroyTexture(winsTexture);

        // Render win rate
        std::string winrate = std::to_string(static_cast<int>(std::round(players[i].winrate)));
        SDL_Surface* winrateSurface = TTF_RenderText_Solid(font, winrate.c_str(), currentColor);
        SDL_Texture* winrateTexture = SDL_CreateTextureFromSurface(renderer, winrateSurface);
        textRect = { 700 - static_cast<int>(winrate.size()) * 17, y, winrateSurface->w, winrateSurface->h };
        SDL_RenderCopy(renderer, winrateTexture, nullptr, &textRect);
        SDL_FreeSurface(winrateSurface);
        SDL_DestroyTexture(winrateTexture);

        // Render favorite strategy
        std::string strategy = players[i].favorite_strategy;
        SDL_Surface* strategySurface = TTF_RenderText_Solid(font, strategy.c_str(), currentColor);
        SDL_Texture* strategyTexture = SDL_CreateTextureFromSurface(renderer, strategySurface);
        textRect = { 1020 - static_cast<int>(strategy.size()) * 15, y, strategySurface->w - static_cast<int>(strategy.size()) * 5, strategySurface->h };
        SDL_RenderCopy(renderer, strategyTexture, nullptr, &textRect);
        SDL_FreeSurface(strategySurface);
        SDL_DestroyTexture(strategyTexture);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}


bool alreadyUsedName (std::vector<Players> &players, std::string name, short playerAmount, short plrOrder) {
    if (playerAmount == 0 ) return false;
    for (int i = 0; i < playerAmount; i++) {
        if (i != plrOrder && players[i].username == name) return true;
    }
    return false;
}

void clearAllPlayerName(std::vector<Players> players, int playerAmount) {
    for (int i = 0; i < playerAmount; i++) {
        players[i].username = "";
    }
}

std::string evaluateHandStrength(const Cards cards[5]) {
    // Sort cards
    std::array<int, 5> ranks;
    std::array<int, 5> suits;
    for (int i = 0; i < 5; i++) {
        ranks[i] = cards[i].rank;
        suits[i] = cards[i].suit;
    }

    // Sort ranks
    std::sort(ranks.begin(), ranks.end());

    // đếm ranks and suits
    std::map<int, int> rankCount;
    std::map<int, int> suitCount;
    for (int i = 0; i < 5; i++) {
        rankCount[ranks[i]]++;
        suitCount[suits[i]]++;
    }

    bool isFlush = (suitCount.size() == 1); // All cards have the same suit
    bool isStraight = (ranks[4] - ranks[0] == 4) && (rankCount.size() == 5);

    // Truong hop dac biet: Ace-low straight (A, 2, 3, 4, 5)
    if (rankCount.size() == 5 && ranks[4] == Cards::ACE && ranks[0] == Cards::TWO &&
        ranks[1] == Cards::THREE && ranks[2] == Cards::FOUR && ranks[3] == Cards::FIVE) {
        isStraight = true;
    }

    // check card strength của player
    if (isStraight && isFlush) {
        return "Straight Flush";
    }
    if (rankCount.size() == 2) {
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
    // Gía trị của các hand strength để so sánh
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
    // update số lần strategy của player
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

    // Tìm strategy
    auto best_strategy = std::max_element(
        strategies.begin(),
        strategies.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first; // Compare values
        }
    );

    // nếu không có strategy thì skip player này
    if (best_strategy->first == 0) continue;


    player.favorite_strategy = best_strategy->second;
    }
}

bool compareHands(const Cards hand1[5], const Cards hand2[5], const std::string& strength1, const std::string& strength2) {
    int rank1 = handRank(strength1);
    int rank2 = handRank(strength2);
    if (rank1 != rank2) return rank1 > rank2;

    std::array<int, 5> ranks1, ranks2;
    for (int i = 0; i < 5; ++i) {
        ranks1[i] = hand1[i].rank;
        ranks2[i] = hand2[i].rank;
    }

    //sort rank
    std::sort(ranks1.rbegin(), ranks1.rend());
    std::sort(ranks2.rbegin(), ranks2.rend());
    
    // So sánh hand strength
    if (strength1 == "Straight Flush" || strength1 == "Straight") {
        // So sánh card mạnh nhất trong sảnh
        return ranks1[0] > ranks2[0];
    }

    if (strength1 == "Four of a Kind") {
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
        for (int i = 0; i < 5; ++i) {
            if (ranks1[i] != ranks2[i]) return ranks1[i] > ranks2[i];
        }
    }

    if (strength1 == "Three of a Kind") {
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
        for (int i = 0; i < 5; ++i) {
            if (ranks1[i] != ranks2[i]) return ranks1[i] > ranks2[i];
        }
    }

    // Nếu tất cả giống nhau thì return hòa (false)
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
            bestPlayerIndex = i;
            bestHandStrength = handStrength;
            bestHand = player.cards;
            winners = {static_cast<int>(i)};
        } else if (handRank(handStrength) == handRank(bestHandStrength) &&
                   !compareHands(bestHand, player.cards, bestHandStrength, handStrength)) {
            //Khi có nhiều hơn 1 player
            winners.push_back(i);
        }
    }

    return winners; //return index của player trong mảng Players
}

void Poker(std::vector<Players> &players, int playerAmount) {
    int mark[52] = {0};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 51);
    // Random cards cho players
    for (int i = 0; i < playerAmount; i++) {
        for (int j = 0; j < 5; j++) {
            int random_number;
            do {
                random_number = dist(gen);
            } while (mark[random_number] != 0);

            mark[random_number] = 1; //đánh dấu là bài đã phát
            players[i].card_id[j] = random_number;

            //Tìm lá bài tương ứng với random_number
            int rankIndex = random_number / 4;
            int suitIndex = random_number % 4;
            players[i].cards[j].rank = static_cast<Cards::Rank>(rankIndex);
            players[i].cards[j].suit = static_cast<Cards::Suit>(suitIndex);
        }
    }

}

// Chat gpt helped me to make player have lower chance to win:
void PVE_Poker(std::vector<Players> &players, int playerAmount) {
    int mark[52] = {0};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 51);

    for (int i = 0; i < playerAmount; i++) {
        std::array<int, 4> suitCount = {0, 0, 0, 0}; // Track suit counts
        std::array<int, 13> rankCount = {0};         // Track rank counts

        for (int j = 0; j < 5; j++) {
            int random_number = -1;
            int attempts = 0;

            if (i == 0) { // First player: Aim for diverse ranks and suits
                do {
                    random_number = dist(gen);
                    int rankIndex = random_number / 4;
                    int suitIndex = random_number % 4;

                    // Prefer cards with unique rank and suit
                    if (suitCount[suitIndex] == 0 && rankCount[rankIndex] == 0 && mark[random_number] == 0) {
                        break;
                    }

                    // Fallback to any unused card after too many attempts
                    if (++attempts > 100) {
                        while (mark[random_number] != 0) {
                            random_number = dist(gen);
                        }
                        break;
                    }
                } while (true);
            } else { // Other players: Increase chances for strong hands
                if (j < 3) { 
                    // Prioritize cards for flush or straight
                    int targetSuit = dist(gen) % 4;
                    int targetRank = (j == 0) ? dist(gen) % 10 : (players[i].cards[j - 1].rank + 1) % 13;

                    do {
                        random_number = targetRank * 4 + targetSuit; // Generate card for flush/straight
                        if (random_number >= 0 && random_number < 52 && mark[random_number] == 0) {
                            break;
                        }

                        if (++attempts > 100) {
                            while (mark[random_number] != 0) {
                                random_number = dist(gen);
                            }
                            break;
                        }
                    } while (true);
                } else if (j == 3) { 
                    // Aim for pairs or three-of-a-kind
                    int targetRank = players[i].cards[0].rank; // Match first card's rank
                    int targetSuit = dist(gen) % 4;

                    do {
                        random_number = targetRank * 4 + targetSuit;
                        if (random_number >= 0 && random_number < 52 && mark[random_number] == 0) {
                            break;
                        }

                        if (++attempts > 100) {
                            while (mark[random_number] != 0) {
                                random_number = dist(gen);
                            }
                            break;
                        }
                    } while (true);
                } else { 
                    // Random card for variety
                    do {
                        random_number = dist(gen);
                    } while (mark[random_number] != 0);
                }
            }

            // Mark card as used
            mark[random_number] = 1;
            players[i].card_id[j] = random_number;

            // Map card_id to rank and suit
            int rankIndex = random_number / 4;
            int suitIndex = random_number % 4;
            players[i].cards[j].rank = static_cast<Cards::Rank>(rankIndex);
            players[i].cards[j].suit = static_cast<Cards::Suit>(suitIndex);

            // Update counts
            suitCount[suitIndex]++;
            rankCount[rankIndex]++;
        }
    }
}

