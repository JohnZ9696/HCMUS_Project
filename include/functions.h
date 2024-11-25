#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "Poker/PokerPlayerStats.h"
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
