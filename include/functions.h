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
#include "Data.h"
#include <algorithm>
#define WIDTH 1200
#define HEIGHT 680



bool init(SDL_Window** window, SDL_Renderer** renderer); //Tao init

void create_Window(SDL_Window*& window, SDL_Renderer*& renderer); //Tạo window

SDL_Texture* loadTexture(const std::string &path, SDL_Renderer* renderer); //Load texture cho image

void close(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* current_image, Mix_Chunk* current_sound, const int& all); //giải phóng window và image texture

bool isMouseInside(const SDL_Rect& buttonRect, int mouseX, int mouseY); //Kiem tra xem con trỏ chuôt có đang trong nut button

void handleHoverSound(bool& isHovering, bool& wasHovering, Mix_Chunk* hoverSound); //Xu li hover sound cua button

void closeText(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font); // Giai phong text

SDL_Texture* renderText(const std::string& text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer); // render text

void renderTextInput(SDL_Renderer* renderer, TTF_Font* font, const std::string& text, SDL_Color color, SDL_Rect rect); //render text khi nhap du lieu

bool alreadyUsedName (std::vector<Players> &players, std::string name, short playerAmount, short plrOrder); // Kiem tra xem name có trùng hay không

void drawThickOutline(SDL_Renderer* renderer, SDL_Rect& rect, int thickness) {
    // Vẽ outline dày bằng cách vẽ nhiều lớp cạnh
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
    for (int i = 0; i < thickness; ++i) {
        // Vẽ từng đường viền dày dần theo lớp
        SDL_Rect outlineRect = {rect.x - i-2, rect.y - i-2, rect.w + 2 * i+4, rect.h + 2 * i+4};
        SDL_RenderDrawRect(renderer, &outlineRect);
    }
}

bool cmp(const Player_Data& a, const Player_Data& b) {
    return a.wins > b.wins;
}
void loadData(std::vector<Player_Data> &players_data) {
    players_data.resize(0);
    std::ifstream fin;
    while (!fin.is_open()) fin.open("include/Data/data.txt");
    if (!fin.is_open()) {
        std::cerr << "Error occured when opening data!";
        SDL_Quit();
        return;
    }

    int idx = 0;
    std::string tmp;
    while (getline(fin, tmp, ' ')) {
        Player_Data player;
        players_data.push_back(player);
        players_data[idx].username = tmp;
        getline(fin, tmp, ' ');
        players_data[idx].wins = stoi(tmp);
        getline(fin, tmp, '\n');
        players_data[idx].battles = stoi(tmp); 
        players_data[idx].winrate = std::round(static_cast<float>(players_data[idx].wins)/static_cast<float>(players_data[idx].battles)*100);
        idx++;
    }
    fin.close();
    sort(players_data.begin(), players_data.end(), cmp);
}

void saveData(std::vector<Player_Data> &players_data) {
    std::fstream fout("include/Data/data.txt", std::ios::out);
    if (!fout.is_open()) {
        std::cout << "Error occured when saving data!";
        return;
    }

    for (int i = 0; i < players_data.size(); i++) {
        fout << players_data[i].username << ' ';
        fout << players_data[i].wins << ' ';
        fout << players_data[i].battles << '\n';
    }
    fout.close();
}

void updateData(std::vector<Player_Data> &players_data) {
    saveData(players_data);
    loadData(players_data);
}

bool playerAlreadyInData (std::vector<Player_Data> players, std::string plr_name) {
    for (auto player : players) {
        if (player.username == plr_name) return true;
    }
    return false;
}

void createNewPlayerInData(std::vector<Player_Data> &players, std::string plr_name) {
    Player_Data newPlayer;
    newPlayer.username = plr_name;

    players.push_back(newPlayer);
}

void renderOverallLeaderBoardScroll(SDL_Renderer* renderer, TTF_Font* font, int scrollOffset, std::vector<Player_Data> players, int playerAmount, SDL_Texture* top1Crown) {
    int lineHeight = 70;

    // Size cua scroll frame
    SDL_Rect viewport = { 70, 200, 1100, 400 };
    SDL_RenderSetClipRect(renderer, &viewport);

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 0);
    SDL_RenderFillRect(renderer, &viewport);

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color yellowColor = { 255, 255, 0, 255 };
    SDL_Color silverColor = { 210, 210, 210, 255 };
    SDL_Color bronzeColor = { 205, 127, 50, 255 };

    int highestWins = 0, secondHighWins = 0, thirdHighWins = 0;

    for (const auto& player : players) {
        if (player.wins > highestWins) {
            thirdHighWins = secondHighWins;
            secondHighWins = highestWins;
            highestWins = player.wins;
        }
        else if (player.wins > secondHighWins && player.wins != highestWins) {
            thirdHighWins = secondHighWins;
            secondHighWins = player.wins;
        } 
        else if (player.wins > thirdHighWins && player.wins != secondHighWins) {
            thirdHighWins = player.wins;
        }
    }
    SDL_Rect textRect, crownRect;
    for (int i = 0; i < playerAmount; ++i) {
        int y = 200 + i * lineHeight - scrollOffset;

        if (y + lineHeight < viewport.y || y > viewport.y + viewport.h) {
            continue;
        }

        SDL_Color color = textColor;
        // Render rank
        std::string rank = std::to_string(i + 1);
        if (players[i]. wins == secondHighWins) {
            color = silverColor;
            SDL_Surface* rankSurface = TTF_RenderText_Solid(font, rank.c_str(), silverColor);
            SDL_Texture* rankTexture = SDL_CreateTextureFromSurface(renderer, rankSurface);
            textRect = { 155 - static_cast<int>(rank.size()) * 15, y, rankSurface->w, rankSurface->h };
            SDL_RenderCopy(renderer, rankTexture, nullptr, &textRect);
            SDL_FreeSurface(rankSurface);
            SDL_DestroyTexture(rankTexture);
        }
        else if (players[i].wins == thirdHighWins) {
             color = bronzeColor;
            SDL_Surface* rankSurface = TTF_RenderText_Solid(font, rank.c_str(), bronzeColor);
            SDL_Texture* rankTexture = SDL_CreateTextureFromSurface(renderer, rankSurface);
            textRect = { 155 - static_cast<int>(rank.size()) * 15, y, rankSurface->w, rankSurface->h };
            SDL_RenderCopy(renderer, rankTexture, nullptr, &textRect);
            SDL_FreeSurface(rankSurface);
            SDL_DestroyTexture(rankTexture);
        } 
        else if (players[i].wins == highestWins) {
             color = yellowColor;
            // Render crown cho winner
            crownRect = { 120, y - 5, 65, 50 };
            SDL_RenderCopy(renderer, top1Crown, nullptr, &crownRect);
        }
        else {
            SDL_Surface* rankSurface = TTF_RenderText_Solid(font, rank.c_str(), textColor);
            SDL_Texture* rankTexture = SDL_CreateTextureFromSurface(renderer, rankSurface);
            textRect = { 155 - static_cast<int>(rank.size()) * 15, y, rankSurface->w, rankSurface->h };
            SDL_RenderCopy(renderer, rankTexture, nullptr, &textRect);
            SDL_FreeSurface(rankSurface);
            SDL_DestroyTexture(rankTexture);
        }

        // Render username
        std::string username = players[i].username;
        SDL_Surface* usernameSurface = TTF_RenderText_Solid(font, username.c_str(), color);
        SDL_Texture* usernameTexture = SDL_CreateTextureFromSurface(renderer, usernameSurface);
        textRect = { 450 - static_cast<int>(username.size()) * 15, y, usernameSurface->w, usernameSurface->h };
        SDL_RenderCopy(renderer, usernameTexture, nullptr, &textRect);
        SDL_FreeSurface(usernameSurface);
        SDL_DestroyTexture(usernameTexture);

        // Render wins
        std::string wins = std::to_string(players[i].wins);
        SDL_Surface* winsSurface = TTF_RenderText_Solid(font, wins.c_str(), color);
        SDL_Texture* winsTexture = SDL_CreateTextureFromSurface(renderer, winsSurface);
        textRect = { 765 - static_cast<int>(wins.size()) * 19, y, winsSurface->w, winsSurface->h };
        SDL_RenderCopy(renderer, winsTexture, nullptr, &textRect);
        SDL_FreeSurface(winsSurface);
        SDL_DestroyTexture(winsTexture);

        // Render win rate
        std::string winrate = std::to_string(static_cast<int>(std::round(players[i].winrate)));
        SDL_Surface* winrateSurface = TTF_RenderText_Solid(font, winrate.c_str(), color);
        SDL_Texture* winrateTexture = SDL_CreateTextureFromSurface(renderer, winrateSurface);
        textRect = { 1005 - static_cast<int>(winrate.size()) * 17, y, winrateSurface->w, winrateSurface->h };
        SDL_RenderCopy(renderer, winrateTexture, nullptr, &textRect);
        SDL_FreeSurface(winrateSurface);
        SDL_DestroyTexture(winrateTexture);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
        
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
