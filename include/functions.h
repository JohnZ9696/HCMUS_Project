#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "Poker/PokerPlayerStats.h"
#include <unordered_set>
#include <cmath>
#define WIDTH 1200
#define HEIGHT 680

bool init(SDL_Window** window, SDL_Renderer** renderer); //Tao init

void create_Window(SDL_Window*& window, SDL_Renderer*& renderer); //Tạo window

SDL_Texture* loadTexture(const std::string &path, SDL_Renderer* renderer); //Load texture cho image

void close(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* current_image, Mix_Chunk* current_sound, const int& all); //giải phóng window và image texture

bool isMouseInside(const SDL_Rect& buttonRect, int mouseX, int mouseY); //Kiem tra xem con trỏ chuôt có đang trong nut button

void handleHoverSound(bool& isHovering, bool& wasHovering, Mix_Chunk* hoverSound); 

void closeText(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font);

SDL_Texture* renderText(const std::string& text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer);

void renderTextInput(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, SDL_Rect rect);

bool alreadyUsedName (std::vector<Players> &players, std::string name, short playerAmount, short plrOrder);


void renderScrollableContent(SDL_Renderer* renderer, TTF_Font* font, int scrollOffset, std::vector<Players> players, int playerAmount, int cardAmount, SDL_Texture* cardTextures[], std::vector<int> winners_idx) {
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
        handStrengthRect = { cardRect.x + 90, y, handStrengthSurface->w - static_cast<int>(handStrength.length()+text.length()-2)*2, handStrengthSurface->h};
        SDL_RenderCopy(renderer, handStrengthTexture, nullptr, &handStrengthRect);

        // Free resources
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
        SDL_FreeSurface(handStrengthSurface);
        SDL_DestroyTexture(handStrengthTexture);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}

bool cmp(const Players& a, const Players& b) {
    return a.wins > b.wins; // Sort in descending order based on score
}

void renderLeaderBoardScroll(SDL_Renderer* renderer, TTF_Font* font, int scrollOffset, std::vector<Players> players, int playerAmount, std::vector<int> winners_idx, SDL_Texture* top1Crown) {
    int lineHeight = 70;

    // Clip content to the frame's area
    SDL_Rect viewport = { 70, 200, 1100, 400 };
    SDL_RenderSetClipRect(renderer, &viewport);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 0);
    SDL_RenderFillRect(renderer, &viewport);

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color winnerColor = { 255, 255, 0, 255 };

    // Use a set for faster winner checks
    std::unordered_set<int> winners(winners_idx.begin(), winners_idx.end());

    SDL_Rect textRect, crownRect;
    for (int i = 0; i < playerAmount; ++i) {
        int y = 200 + i * lineHeight - scrollOffset;

        // Skip players entirely outside the viewport
        if (y + lineHeight < viewport.y || y > viewport.y + viewport.h) {
            continue;
        }

        bool isWinner = winners.find(i) != winners.end();
        SDL_Color currentColor = isWinner ? winnerColor : textColor;

        // Render rank
        std::string rank = std::to_string(i + 1);
        if (i > 0) {
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


bool alreadyUsedName (std::vector<Players> &players, std::string name, short playerAmount, short plrOrder) {
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

void handleHoverSound(bool& isHovering, bool& wasHovering, Mix_Chunk* hoverSound) {
    if (isHovering && !wasHovering) {
        Mix_PlayChannel(-1, hoverSound, 0);
    }
    wasHovering = isHovering;
}

bool isMouseInside(const SDL_Rect& buttonRect, int mouseX, int mouseY) {
    return (mouseX > buttonRect.x && mouseX < buttonRect.x + buttonRect.w &&
            mouseY > buttonRect.y && mouseY < buttonRect.y + buttonRect.h);
}

bool init(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *window = SDL_CreateWindow("HCMUS Project", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return false;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

void create_Window(SDL_Window*& window, SDL_Renderer*& renderer) {
    if (!init(&window, &renderer)) {
        std::cerr << "Failed to initialize!" << std::endl;
        return;
    }
}

void closeText(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font) {
    if (font) TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}


SDL_Texture* renderText(const std::string& text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface == nullptr) {
        std::cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);

    return texture;
}

void renderTextInput(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, SDL_Rect rect) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_RenderCopy(renderer, texture, nullptr, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

SDL_Texture* loadTexture(const std::string &path, SDL_Renderer* renderer) {

    SDL_Texture* newTexture = nullptr;
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == nullptr) {
        std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
        return nullptr;
    } else {
        newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);
        if (newTexture == nullptr) {
            std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
        }
    }
    return newTexture;
}

void close(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* current_image, Mix_Chunk* current_sound, Mix_Music* backgroundMusic, const int& all) {
    if (all == 1) { //Nếu all == 1 thì giải phóng cả ảnh hiện tại và window
        Mix_FreeMusic(backgroundMusic);  // Giải phóng nhạc nền
        Mix_CloseAudio();
        SDL_DestroyTexture(current_image);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        Mix_FreeChunk(current_sound);
        IMG_Quit();
        SDL_Quit();
    }
    else if (all == 2) { //Nếu all == 2 thì chỉ giải phóng texture của ảnh hiện tại
        SDL_DestroyTexture(current_image);
    }
    else if (all == 3) {
        Mix_FreeChunk(current_sound);
    }
    else if (all == 4) {
        Mix_FreeMusic(backgroundMusic);  // Giải phóng nhạc nền
        Mix_CloseAudio();
    }
}
