#include "Baccarat/BaccaratPlayerStats.h"
#include <vector>
#include <random>
#include <algorithm>
#include <iostream>

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
void renderBaccaratResultBoard(SDL_Renderer* renderer, TTF_Font* font, int scrollOffset, std::vector<Baccarat_Players> players, int playerAmount, int cardAmount, SDL_Texture* cardTextures[], std::vector<int> winners_idx) {
    int lineHeight = 125;   

    // Size của scroll frame
    SDL_Rect viewport = { 70, 140, 1090, 470 };
    SDL_RenderSetClipRect(renderer, &viewport);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 0);
    SDL_RenderFillRect(renderer, &viewport);

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color winnerColor = { 255, 255, 0, 255 };

    std::unordered_set<int> winners(winners_idx.begin(), winners_idx.end());

    SDL_Rect textRect, cardRect, handStrengthRect;
    for (int i = 0; i < playerAmount; ++i) {
        int y = 170 + i * lineHeight - scrollOffset;

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
        std::string handStrength;
        if (players[i].handType == "Regular") handStrength = "(score: " + std::to_string(players[i].score) + ')';
        else handStrength = '('+ players[i].handType + ')';
        SDL_Surface* handStrengthSurface = TTF_RenderText_Solid(font, handStrength.c_str(), currentColor);
        SDL_Texture* handStrengthTexture = SDL_CreateTextureFromSurface(renderer, handStrengthSurface);
        handStrengthRect = { cardRect.x + 90, y, handStrengthSurface->w - static_cast<int>(handStrength.length()*3+text.length()*3-2)+30, handStrengthSurface->h};
        SDL_RenderCopy(renderer, handStrengthTexture, nullptr, &handStrengthRect);

        // giải phóng memory
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(handStrengthSurface);
        SDL_DestroyTexture(handStrengthTexture);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}

bool Baccarat_cmp(const Baccarat_Players& a, const Baccarat_Players& b) {
    return a.wins > b.wins;
    //sort theo số lần win
}

void renderBaccaratLeaderBoardScroll(SDL_Renderer* renderer, TTF_Font* font, int scrollOffset, std::vector<Baccarat_Players> players, int playerAmount, std::vector<int> winners_idx, SDL_Texture* top1Crown) {
    int lineHeight = 70;

    // size của scroll frame
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
            // Render crown for top 1
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

int calculateScore(Baccarat_Players& player, int cardCount) { 
    int score = 0;
    int ba_tay = 0;
    bool sap = true, lieng = false;
    std::vector<int> ranks;

    for (int i = 0; i < cardCount; ++i) {
        int rankValue = (player.cards[i].rank >= Baccarat_Cards::TEN) ? 0 : player.cards[i].rank;
        score += rankValue;
        ranks.push_back(player.cards[i].rank);

        // check nếu bài của player là ba tây
        if (player.cards[i].rank == Baccarat_Cards::JACK || 
            player.cards[i].rank == Baccarat_Cards::QUEEN || 
            player.cards[i].rank == Baccarat_Cards::KING) {
            ba_tay++;
        }

        // check nếu là sáp
        if (i > 0 && player.cards[i].rank != player.cards[i - 1].rank) {
            sap = false;
        }
    }

    // check nếu là liêng
    std::sort(ranks.begin(), ranks.end());
    if ((ranks[2] - ranks[1] == 1 && ranks[1] - ranks[0] == 1) ||  // Straight sequence
        (ranks[0] == Baccarat_Cards::ACE && ranks[1] == Baccarat_Cards::TWO && ranks[2] == Baccarat_Cards::KING)) {  // Wrap-around (K-A-2)
        lieng = true;
    }

    // Cac truong hop dac biet
    if (ba_tay == 3) {
        player.handType = "Ba Tay";
    } else if (sap) {
        player.handType = "Sap";
        score = 10;
    } else if (lieng) {
        player.handType = "Lieng";
        score = 9;
    } else {
        player.handType = "Regular";
    }

    return score % 10; // chỉ lấy hàng đơn vị
}
void updateHandtype(int plr_idx, std::vector<Baccarat_Players>& players) {
    if (players[plr_idx].handType == "Regular") players[plr_idx].regular++;
    else if (players[plr_idx].handType == "Sap") players[plr_idx].sap++;
    else if (players[plr_idx].handType == "Lieng") players[plr_idx].lieng++;
    else if (players[plr_idx].handType == "Ba Tay") players[plr_idx].ba_tay++;
}

// Tìm ra favorite handtype 
std::string getMostFrequentHandType(const Baccarat_Players& player) {
    int maxCount = player.regular;
    std::string mostFrequent = "Regular";

    if (player.ba_tay > maxCount) {
        maxCount = player.ba_tay;
        mostFrequent = "Ba Tay";
    }
    if (player.lieng > maxCount) {
        maxCount = player.lieng;
        mostFrequent = "Lieng";
    }
    if (player.sap > maxCount) {
        maxCount = player.sap;
        mostFrequent = "Sap";
    }

    return mostFrequent;
}

void determineWinner(std::vector<Baccarat_Players>& players, std::vector<int>& winners) {
    int highestScore = -1;
    winners.resize(0);
    // tìm điểm cao nhất
    for (const auto& player : players) {
        if (player.score > highestScore) {
            highestScore = player.score;
        }
    }
    // tìm player có số điểm bằng highest score
    for (int i = 0; i < players.size(); ++i) {
        if (players[i].score == highestScore) {
            winners.push_back(i);
        }
    }

    // Update hand type và wins của winner 
    for (const auto& winner : winners) {
        players[winner].wins += 1;
        updateHandtype(winner, players);
        players[winner].favorite_strategy = getMostFrequentHandType(players[winner]);
    }
}
void sortWinner(std::vector<Baccarat_Players> players, std::vector<int>& winners) {
    int highestScore = -1;
    winners.resize(0);
    for (const auto& player : players) {
        if (player.score > highestScore) {
            highestScore = player.score;
        }
    }
    //sort winner theo thứ tự để hiển thị trong leaderboard
    for (int i = 0; i < players.size(); ++i) {
        if (players[i].score == highestScore) {
            winners.push_back(i);
        }
    }
}

void Baccarat(std::vector<Baccarat_Players>& players, int playerAmount) {
    int mark[52] = {0};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 51);

    for (int i = 0; i < playerAmount; i++) {
        for (int j = 0; j < 3; j++) {
            int random_number;
            do {
                random_number = dist(gen);
            } while (mark[random_number] != 0);

            mark[random_number] = 1;
            players[i].card_id[j] = random_number;

            int rankIndex = random_number / 4 + 2;
            int suitIndex = random_number % 4;
            players[i].cards[j].rank = (rankIndex == 14) ? Baccarat_Cards::ACE : static_cast<Baccarat_Cards::Rank>(rankIndex);
            players[i].cards[j].suit = static_cast<Baccarat_Cards::Suit>(suitIndex);
        }
    }
    
    for (auto& player : players) {
        player.score = calculateScore(player, 3);
    }

}

//Chat gpt helped me to make player have lower chance to win against bots
void PVE_Baccarat(std::vector<Baccarat_Players>& players, int playerAmount) {
    // Initialize random number generator
    int mark[52] = {0};
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 51);

    // Assign low cards randomly to the first player
    std::vector<int> lowRanks = {2, 3, 4, 5, 6, 7}; // Low ranks to select from
    std::shuffle(lowRanks.begin(), lowRanks.end(), gen);

    Baccarat_Cards lowCards[3];
    for (int i = 0; i < 3; ++i) {
        int rank = lowRanks[i]; // Select a random rank
        int suit = dist(gen) % 4; // Random suit for the rank
        int cardId = (rank - 2) * 4 + suit;

        while (mark[cardId] != 0) { // Avoid duplicates
            suit = dist(gen) % 4;
            cardId = (rank - 2) * 4 + suit;
        }

        lowCards[i] = {static_cast<Baccarat_Cards::Suit>(suit), static_cast<Baccarat_Cards::Rank>(rank)};
        players[0].cards[i] = lowCards[i];
        players[0].card_id[i] = cardId;
        mark[cardId] = 1; // Mark card as used
    }

    // Randomly assign cards to other players
    for (int i = 1; i < playerAmount; i++) {
        for (int j = 0; j < 3; j++) {
            int random_number;
            do {
                random_number = dist(gen);
            } while (mark[random_number] != 0);

            mark[random_number] = 1; // Mark card as used
            players[i].card_id[j] = random_number;

            // Map card_id to rank and suit
            int rankIndex = random_number / 4 + 2; // Corrected rank mapping
            int suitIndex = random_number % 4;
            players[i].cards[j].rank = (rankIndex == 14) ? Baccarat_Cards::ACE : static_cast<Baccarat_Cards::Rank>(rankIndex);
            players[i].cards[j].suit = static_cast<Baccarat_Cards::Suit>(suitIndex);
        }
    }

    // Calculate scores and assign hand types
    for (auto& player : players) {
        player.score = calculateScore(player, 3);
    }
}

