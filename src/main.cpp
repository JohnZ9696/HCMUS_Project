#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include "functions.h"
#include "Poker/Poker.h"

/*
Status numbers:
-1: Loading screen
0: Main menu
1: Choose gameplay
2: Settings
3: Choose PvP or PvE (Gamemode)
4: Enter number of players
5: Enter players' names
*/

/*
Gameplay status numbers:
1: Poker
2: Big Two
3: Baccarat
*/



int main(int argc, char*argv[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (TTF_Init() == -1) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
    }

    create_Window(window, renderer);


    //Loading
    TTF_Font* font = TTF_OpenFont("fonts/loading.ttf", 110); // Thay đường dẫn bằng file font của bạn
    if (font == nullptr) {
        std::cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << std::endl;
        closeText(window, renderer, font);
        return -1;
    }

    SDL_Color textColor = { 255, 255, 255, 255 }; // Màu trắng
    SDL_Texture* textTexture1 = renderText("Loading...", font, textColor, renderer);
    if (textTexture1 == nullptr) {
        closeText(window, renderer, font);
        return -1;
    }

    SDL_Rect textRect;
    textRect.x = 420; // Vị trí x
    textRect.y = 280; // Vị trí y
    SDL_QueryTexture(textTexture1, nullptr, nullptr, &textRect.w, &textRect.h); // Lấy kích thước của text
    
   //Poker 
    SDL_Rect cardRect1 = { 130, 250, 150, 250 };
    SDL_Rect cardRect2 = { 330, 250, 150, 250 };
    SDL_Rect cardRect3 = { 530, 250, 150, 250 };
    SDL_Rect cardRect4 = { 730, 250, 150, 250 };
    SDL_Rect cardRect5 = { 930, 250, 150, 250 };
    
    SDL_Rect yourCardsRect = { 270, -130, 660, 580 }; 
    SDL_Texture* yourCards_Texture = loadTexture("image/yourcards.png", renderer);
    if (yourCards_Texture == nullptr) {
        std::cerr << "Failed to load title texture!" << std::endl;
        close(window, renderer, yourCards_Texture, nullptr, nullptr, 1);
        return -1;
    }


    SDL_Rect flipAll_buttonRect = { 475, 560, 250, 70 }; 
    SDL_Rect flipAll_hoverRect = { flipAll_buttonRect.x - 10, flipAll_buttonRect.y - 10, flipAll_buttonRect.w + 20, flipAll_buttonRect.h + 20 };
    SDL_Texture* flipAll_Texture = loadTexture("image/flipAll_button.png", renderer);
    if (flipAll_Texture == nullptr) {
        std::cerr << "Failed to load flipAll button texture!" << std::endl;
        close(window, renderer, flipAll_Texture, nullptr, nullptr, 1);
        return -1;
    }

    std::vector<Cards> card(52);

    SDL_Rect next_buttonRect = { 920, 560, 200, 100 }; 
    SDL_Rect next_buttonHoverRect = { next_buttonRect.x - 10, next_buttonRect.y - 10, next_buttonRect.w + 10, next_buttonRect.h + 20 };

    SDL_Texture* next_Texture = loadTexture("image/next_button.png", renderer);
    if (next_Texture == nullptr) {
        std::cerr << "Failed to load next button texture!" << std::endl;
        close(window, renderer, next_Texture, nullptr, nullptr, 1);
        return -1;
    }


    //Load texture 52 la bai
    SDL_Texture* cardTextures[52] = { nullptr };

    for (int i = 0; i <= 52; i++) {
        std::string path = "image/cards/card_" + std::to_string(i) + ".png";
        cardTextures[i] = loadTexture(path, renderer);
        if (cardTextures[i] == nullptr) {
            std::cerr << "Failed to load card texture for card " << i << std::endl;

            for (int i = 0; i < 52; ++i) {
                SDL_DestroyTexture(cardTextures[i]);
                cardTextures[i] = nullptr;
            }

            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            IMG_Quit();
            SDL_Quit();
            return -1;
        }
        SDL_RenderCopy(renderer, textTexture1, nullptr, &textRect); // Vẽ text lên màn hình
        SDL_RenderPresent(renderer);
    }

    //Background image
    SDL_Texture* backgroundTexture = loadTexture("image/background.jpg", renderer);
    if (backgroundTexture == nullptr) {
        std::cerr << "Failed to load background texture!" << std::endl;
        close(window, renderer, backgroundTexture, nullptr, nullptr, 0);
        return -1;
    }

    //Game title image
    SDL_Rect title_Rect = { 275, 50, 650, 200 }; 
    SDL_Texture* title_Texture = loadTexture("image/Title.png", renderer);
    if (title_Texture == nullptr) {
        std::cerr << "Failed to load title texture!" << std::endl;
        close(window, renderer, title_Texture, nullptr, nullptr, 1);
        return -1;
    }

    // Game's main menu
    SDL_Rect play_buttonRect = { 475, 290, 260, 100 }; 
    SDL_Rect play_hoverRect = { play_buttonRect.x - 10, play_buttonRect.y - 10, play_buttonRect.w + 20, play_buttonRect.h + 20 };
    SDL_Texture* play_buttonTexture = loadTexture("image/Play_button.png", renderer);
    if (play_buttonTexture == nullptr) {
        std::cerr << "Failed to load play button texture!" << std::endl;
        close(window, renderer, play_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Rect settings_buttonRect = { 475, 420, 260, 100 }; 
    SDL_Rect settings_hoverRect = { settings_buttonRect.x - 10, settings_buttonRect.y - 10, settings_buttonRect.w + 20, settings_buttonRect.h + 20 };
    SDL_Texture* settings_buttonTexture = loadTexture("image/Settings_button.png", renderer);
    if (settings_buttonTexture == nullptr) {
        std::cerr << "Failed to load settings button texture!" << std::endl;
        close(window, renderer, settings_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }
    
    SDL_Rect exit_buttonRect = { 475, 550, 260, 100 }; 
    SDL_Rect exit_hoverRect = { exit_buttonRect.x - 10, exit_buttonRect.y - 10, exit_buttonRect.w + 20, exit_buttonRect.h + 20 };
    SDL_Texture* exit_buttonTexture = loadTexture("image/Exit_button.png", renderer);
    if (exit_buttonTexture == nullptr) {
        std::cerr << "Failed to load exit button texture!" << std::endl;
        close(window, renderer, exit_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }
    
    Mix_Chunk* buzzerSound = Mix_LoadWAV("sound/buzzer.mp3");
    if (buzzerSound == nullptr) {
        std::cerr << "Failed to load buzzer sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
        close(window, renderer, backgroundTexture, buzzerSound, nullptr, 1);
        return -1;
    }
    

    //Hover sound
    Mix_Chunk* hoverSound = Mix_LoadWAV("sound/button_hover.mp3");
    if (hoverSound == nullptr) {
        std::cerr << "Failed to load hover sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
        close(window, renderer, backgroundTexture, hoverSound, nullptr, 1);
        return -1;
    }
    
    //Flip card sound effect
    Mix_Chunk* flipcard = Mix_LoadWAV("sound/flipcard.mp3");
    if (flipcard == nullptr) {
        std::cerr << "Failed to load flipcard sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
        close(window, renderer, backgroundTexture, flipcard, nullptr, 1);
        return -1;
    }

    //Click sound
    Mix_Chunk* clickSound = Mix_LoadWAV("sound/button_click.wav");
    if (clickSound == nullptr) {
        std::cerr << "Failed to load click sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
        close(window, renderer, backgroundTexture, clickSound, nullptr, 1);
        return -1;
    }

    Mix_Chunk* checkbox_clickSound = Mix_LoadWAV("sound/check_box.mp3");
    if (checkbox_clickSound == nullptr) {
        std::cerr << "Failed to load click sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
        close(window, renderer, backgroundTexture, checkbox_clickSound, nullptr, 1);
        return -1;
    }


    //Background music
    Mix_Music* backgroundMusic = Mix_LoadMUS("sound/background_music.mp3");
    if (backgroundMusic == nullptr) {
        std::cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        close(window, renderer, nullptr, nullptr, nullptr, 1);
        return -1;
    }

    Mix_VolumeMusic(40);

    if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
        std::cerr << "Failed to play background music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        close(window, renderer, nullptr, nullptr, backgroundMusic, 1);
        return -1;
    }

    Mix_Music* PokerSound = Mix_LoadMUS("sound/Poker.mp3");
    if (PokerSound == nullptr) {
        std::cerr << "Failed to Poker sound! SDL_mixer Error: " << Mix_GetError() << std::endl;
        close(window, renderer, nullptr, nullptr, nullptr, 1);
        return -1;
    }

    //Nhap nguoi choi
    TTF_Font* main_font = TTF_OpenFont("fonts/main_font.ttf", 12);
    if (!main_font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    TTF_Font* Pixel_font = TTF_OpenFont("fonts/Pixel.ttf", 20);
    if (!Pixel_font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    TTF_Font* SuperPixel_font = TTF_OpenFont("fonts/Super_Pixel.ttf", 40);
    if (!SuperPixel_font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_Color whiteColor = { 255, 255, 255, 255 }; 
    SDL_Color yellowColor = { 230, 230, 0, 255 }; 
    SDL_Color brownColor = { 139, 69, 19, 255 };

    SDL_Rect warningInvalidNums_Rect;
    warningInvalidNums_Rect.x = 315;
    warningInvalidNums_Rect.y = 495;

    SDL_Texture* warnInvalidNums = renderText("You entered an invalid number!", Pixel_font, yellowColor, renderer);
    SDL_QueryTexture(warnInvalidNums, nullptr, nullptr, &warningInvalidNums_Rect.w, &warningInvalidNums_Rect.h);
    if (warnInvalidNums == nullptr) {
        closeText(window, renderer, Pixel_font);
        return -1;
    }

    SDL_Rect warningInvalidUsername_Rect;
    warningInvalidUsername_Rect.x = 240;
    warningInvalidUsername_Rect.y = 500;
    SDL_Texture* warnInvalidUsername = renderText("This name is invalid or has been taken!", Pixel_font, yellowColor, renderer);
    SDL_QueryTexture(warnInvalidUsername, nullptr, nullptr, &warningInvalidUsername_Rect.w, &warningInvalidUsername_Rect.h);
    if (warnInvalidUsername == nullptr) {
        closeText(window, renderer, Pixel_font);
        return -1;
    }



    SDL_Rect enterUsername_Rect;
    enterUsername_Rect.x = 315;
    enterUsername_Rect.y = 200;
    SDL_Texture* enterUsername_Text = renderText("Enter your username:", SuperPixel_font, brownColor, renderer);
    SDL_QueryTexture(enterUsername_Text, nullptr, nullptr, &enterUsername_Rect.w, &enterUsername_Rect.h);
    if (enterUsername_Text == nullptr) {
        closeText(window, renderer, SuperPixel_font);
        return -1;
    }


    SDL_Rect enterPlayerNums_Rect = { 280, 30, 600, 600 };
    SDL_Texture* enterPlayerNums_Texture = loadTexture("image/enterPlayerNums.png", renderer);
    if (enterPlayerNums_Texture == nullptr) {
        std::cerr << "Failed to load enterPlayerNums texture!" << std::endl;
        close(window, renderer, title_Texture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Rect confirm_Rect = { 470, 535, 260, 100 };
    SDL_Rect confirm_hoverRect = { confirm_Rect.x -10, confirm_Rect.y - 10, confirm_Rect.w + 20, confirm_Rect.h + 20};
    SDL_Texture* confirm_Texture = loadTexture("image/confirm_button.png", renderer);
    if (confirm_Texture == nullptr) {
        std::cerr << "Failed to load confirm texture!" << std::endl;
        close(window, renderer, confirm_Texture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Color textInputColor = { 0, 0, 0, 0 }; 
    SDL_Color inputBoxColor = {235, 235, 235, 0};
    SDL_Color activeBoxColor = {160, 160, 160, 0};

    // Input box
    SDL_Rect inputBox = {558, 345, 70, 80};
    SDL_Rect inputUsernameBox = {390, 285, 400, 70};
    std::string inputText = "";
    std::string inputUsernameText = "Enter here";
    bool inputActive = false;

    SDL_Rect enterPlayerUsernameBoard_Rect = { 170, -10, 850, 600 }; 
    SDL_Texture* enterUsernameBoard_Texture = loadTexture("image/enterUsername.png", renderer);
    if (enterUsernameBoard_Texture == nullptr) {
        std::cerr << "Failed to load enterUsername board texture!" << std::endl;
        close(window, renderer, enterUsernameBoard_Texture, nullptr, nullptr, 1);
        return -1;
    }

    //Setting board
    SDL_Rect settingBoard_Rect = { 425, 80, 360, 510 }; 
    SDL_Texture* settingBoard_Texture = loadTexture("image/Settings_board.png", renderer);
    if (settingBoard_Texture == nullptr) {
        std::cerr << "Failed to load title texture!" << std::endl;
        close(window, renderer, settingBoard_Texture, nullptr, nullptr, 1);
        return -1;
    }

    //Check and uncheck in setting
    bool isPlayingMusic = true;
    bool isPlayingSoundFX = true;

    SDL_Rect checkedSoundFX_buttonRect = { 650, 365, 75, 95 }; //Nút on/off của SoundFX
    SDL_Rect checkedMusic_buttonRect = { 650, 263, 75, 95 }; //Nút on/off của Background music
    SDL_Texture* checkedMusic_buttonTexture = loadTexture("image/checked_box.png", renderer);
    if (checkedMusic_buttonTexture == nullptr) {
        std::cerr << "Failed to load music button texture!" << std::endl;
        close(window, renderer, checkedMusic_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Rect uncheckedSoundFX_buttonRect = { checkedSoundFX_buttonRect.x+5, checkedSoundFX_buttonRect.y+4, checkedSoundFX_buttonRect.w-10, checkedSoundFX_buttonRect.h-5 }; 
    SDL_Rect uncheckedMusic_buttonRect = { checkedMusic_buttonRect.x+5, checkedMusic_buttonRect.y+4, checkedMusic_buttonRect.w-10, checkedMusic_buttonRect.h-5 }; 
    SDL_Texture* uncheckedMusic_buttonTexture = loadTexture("image/unchecked_box.png", renderer);
    if (uncheckedMusic_buttonTexture == nullptr) {
        std::cerr << "Failed to load music button texture!" << std::endl;
        close(window, renderer, uncheckedMusic_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Rect backButton_Rect = { 970, 540, 170, 80 }; 
    SDL_Rect backButton_hoverRect = { backButton_Rect.x - 10, backButton_Rect.y - 10, backButton_Rect.w + 20, backButton_Rect.h + 20 };
    SDL_Texture* backButton_Texture = loadTexture("image/back_button.png", renderer);
    if (backButton_Texture == nullptr) {
        std::cerr << "Failed to load title texture!" << std::endl;
        close(window, renderer, title_Texture, nullptr, nullptr, 1);
        return -1;
    }


    //Status 1
    SDL_Rect gameplaysTitle_Rect = { 400, 80, 400, 100 }; 
    SDL_Texture* gameplaysTitle_Texture = loadTexture("image/gameplays_title.png", renderer);
    if (gameplaysTitle_Texture == nullptr) {
        std::cerr << "Failed to load gameplays title texture!" << std::endl;
        close(window, renderer, gameplaysTitle_Texture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Rect Poker_buttonRect = { 475, 230, 260, 100 }; 
    SDL_Rect Poker_hoverRect = { Poker_buttonRect.x - 10, Poker_buttonRect.y - 10, Poker_buttonRect.w + 20, Poker_buttonRect.h + 20 };
    SDL_Texture* Poker_buttonTexture = loadTexture("image/Poker_button.png", renderer);
    if (Poker_buttonTexture == nullptr) {
        std::cerr << "Failed to load poker button texture!" << std::endl;
        close(window, renderer, Poker_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Rect BigTwo_buttonRect = { 475, 360, 260, 100 }; 
    SDL_Rect BigTwo_hoverRect = { BigTwo_buttonRect.x - 10, BigTwo_buttonRect.y - 10, BigTwo_buttonRect.w + 20, BigTwo_buttonRect.h + 20 };
    SDL_Texture* BigTwo_buttonTexture = loadTexture("image/BlackJack_button.png", renderer);
    if (BigTwo_buttonTexture == nullptr) {
        std::cerr << "Failed to load poker button texture!" << std::endl;
        close(window, renderer, BigTwo_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Rect Baccarat_buttonRect = { 475, 500, 260, 100 }; 
    SDL_Rect Baccarat_hoverRect = { Baccarat_buttonRect.x - 10, Baccarat_buttonRect.y - 10, Baccarat_buttonRect.w + 20, Baccarat_buttonRect.h + 20 };
    SDL_Texture* Baccarat_buttonTexture = loadTexture("image/Baccarat_button.png", renderer);
    if (Baccarat_buttonTexture == nullptr) {
        std::cerr << "Failed to load poker button texture!" << std::endl;
        close(window, renderer, Baccarat_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }

    //Status 3
    SDL_Rect gamemodeTitle_Rect = { 400, 90, 400, 100 }; 
    SDL_Texture* gamemodeTitle_Texture = loadTexture("image/gamemode_title.png", renderer);
    if (gamemodeTitle_Texture == nullptr) {
        std::cerr << "Failed to load gamemode title texture!" << std::endl;
        close(window, renderer, gamemodeTitle_Texture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Rect PVP_buttonRect = { 475, 290, 260, 100 }; 
    SDL_Rect PVP_hoverRect = { PVP_buttonRect.x - 10, PVP_buttonRect.y - 10, PVP_buttonRect.w + 20, PVP_buttonRect.h + 20 };
    SDL_Texture* PVP_buttonTexture = loadTexture("image/PVP_button.png", renderer);
    if (PVP_buttonTexture == nullptr) {
        std::cerr << "Failed to load PVP button texture!" << std::endl;
        close(window, renderer, PVP_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }

    SDL_Rect PVE_buttonRect = { 475, 450, 260, 100 }; 
    SDL_Rect PVE_hoverRect = { PVE_buttonRect.x - 10, PVE_buttonRect.y - 10, PVE_buttonRect.w + 20, PVE_buttonRect.h + 20 };
    SDL_Texture* PVE_buttonTexture = loadTexture("image/PVE_button.png", renderer);
    if (PVE_buttonTexture == nullptr) {
        std::cerr << "Failed to load PVP button texture!" << std::endl;
        close(window, renderer, PVE_buttonTexture, nullptr, nullptr, 1);
        return -1;
    }

    bool quit = false;
    SDL_Event e;

    bool wasHovering = false, wrongAnswer = false, isPVP = false;
    short plrIdSwitch = 0;
    int status = 0;
    int game_status = 0;
    bool isPlaying = false;
    bool Load = false;
    short playerAmount = 0;
    std::vector<Players> players;
    while (!quit) {
        int mouseX, mouseY;
        bool isHovering;
        if (status == 0) {
            isHovering = isMouseInside(play_buttonRect, mouseX, mouseY) || isMouseInside(settings_buttonRect, mouseX, mouseY) || isMouseInside(exit_buttonRect, mouseX, mouseY);
        }
        else if (status == 1) {
            isHovering = isMouseInside(Poker_buttonRect, mouseX, mouseY) || isMouseInside(BigTwo_buttonRect, mouseX, mouseY) || isMouseInside(Baccarat_buttonRect, mouseX, mouseY);
        }
        else if (status == 2) {
            isHovering = isMouseInside(backButton_Rect, mouseX, mouseY);
        }
        else if (status == 3) {
            isHovering = isMouseInside(backButton_Rect, mouseX, mouseY) || isMouseInside(PVP_buttonRect, mouseX, mouseY) || isMouseInside(PVE_buttonRect, mouseX, mouseY) ;
        }
        else if (status == 4) {
            isHovering = isMouseInside(confirm_Rect, mouseX, mouseY);
        }
        else if (status == 5) {
            isHovering = isMouseInside(backButton_Rect, mouseX, mouseY) || isMouseInside(flipAll_buttonRect, mouseX, mouseY);
        }
        if (isHovering && !wasHovering && isPlayingSoundFX) {
            Mix_PlayChannel(-1, hoverSound, 0);
        }
        wasHovering = isHovering;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEMOTION) {
                mouseX = e.motion.x;
                mouseY = e.motion.y;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (status == 0) {
                    if (isMouseInside(exit_buttonRect, mouseX, mouseY)) {
                        quit = true;
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                    }
                    else if (isMouseInside(play_buttonRect, mouseX, mouseY)) { //Nhấn Play button
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 1; 
                    }
                    else if (isMouseInside(settings_buttonRect, mouseX, mouseY)) {  //Nhấn settings button
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 2;
                    }
                }
                else if (status == 1) {
                    
                    if (isMouseInside(Poker_buttonRect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 3;
                        game_status = 1;
                    }
                    else if (isMouseInside(BigTwo_buttonRect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 3;
                        game_status = 2;
                    }
                    else if (isMouseInside(Baccarat_buttonRect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 3;
                        game_status = 3;
                    }
                    else if (isMouseInside(backButton_Rect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 0;
                    }
                    
                }
                else if (status == 2) {
                    if (isMouseInside(uncheckedMusic_buttonRect, mouseX, mouseY) || isMouseInside(checkedMusic_buttonRect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, checkbox_clickSound, 0);
                        if (!isPlayingMusic) {
                            Mix_PlayMusic(backgroundMusic, -1);
                            isPlayingMusic = true;
                        }
                        else {
                            Mix_HaltMusic();
                            isPlayingMusic = false;
                        }
                    }
                    else if (isMouseInside(uncheckedSoundFX_buttonRect, mouseX, mouseY) || isMouseInside(checkedSoundFX_buttonRect, mouseX, mouseY)) {
                        if (!isPlayingSoundFX) {
                            isPlayingSoundFX = true;
                        }
                        else {
                            isPlayingSoundFX = false;
                        }
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, checkbox_clickSound, 0);
                    }
                    else if (isMouseInside(backButton_Rect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 0;
                    }
                }
                else if (status == 3) {
                    if (isMouseInside(backButton_Rect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 1;
                    }
                    else if (isMouseInside(PVP_buttonRect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        isPVP = true;
                        status = 4;
                    }
                    else if (isMouseInside(PVE_buttonRect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        isPVP = false;
                        status = 4;
                    }
                }
                else if (status == 4) {
                    if (isMouseInside(backButton_Rect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 3;
                    }
                    if (isMouseInside(inputBox, mouseX, mouseY) && !inputActive) {
                        inputActive = true;
                        inputText = "";
                    } 
                    else {
                        inputActive = false;
                    }
                    if (isMouseInside(confirm_Rect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        int nums = 0;
                        if (inputText != "") nums = stoi(inputText);
                        if (nums <= 1 || nums >= 11) {
                            wrongAnswer = true;
                            if (buzzerSound) Mix_PlayChannel(-1, buzzerSound, 0);
                        }
                        else {
                            wrongAnswer = false;
                            playerAmount = nums;
                            players.resize(playerAmount);
                            if (!isPVP) {
                                for (int i = 1; i < playerAmount; i++) {
                                    players[i].username = "Bot " + std::to_string(i);
                                }
                            }
                            status = 5;
                        }
                    }
                }
                else if (status == 5) {
                    if (isMouseInside(backButton_Rect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        status = 4;
                    }
                    if (isMouseInside(inputUsernameBox, mouseX, mouseY) && !inputActive) {
                        inputActive = true;
                        inputUsernameText = "";
                    } 
                    else {
                        inputActive = false;
                    }
                    if (isMouseInside(confirm_Rect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        if (inputUsernameText != "Enter here" && inputUsernameText != "" && !alreadyUsedName(players, inputUsernameText, playerAmount, plrIdSwitch)) {
                            players[plrIdSwitch++].username = inputUsernameText;
                            if (plrIdSwitch >= playerAmount && isPVP || !isPVP && plrIdSwitch <= 1) {
                                status = 6;
                                plrIdSwitch = 0;
                            }
                            inputUsernameText = "Enter here";
                        }
                        else {
                            wrongAnswer = true;
                            if (isPlayingSoundFX) Mix_PlayChannel(-1, buzzerSound, 0);
                        }
                    }
                }
                else if (status == 6 && game_status == 1) {
                    if (isMouseInside(cardRect1, mouseX, mouseY) && !players[plrIdSwitch].openingCards[0]) {
                        players[plrIdSwitch].openingCards[0] = 1;
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, flipcard, 0);
                    }
                    else if (isMouseInside(cardRect2, mouseX, mouseY) && !players[plrIdSwitch].openingCards[1]) {
                        players[plrIdSwitch].openingCards[1] = 1;
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, flipcard, 0);
                    }
                    else if (isMouseInside(cardRect3, mouseX, mouseY) && !players[plrIdSwitch].openingCards[2]) {
                        players[plrIdSwitch].openingCards[2] = 1;
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, flipcard, 0);
                    }
                    else if (isMouseInside(cardRect4, mouseX, mouseY) && !players[plrIdSwitch].openingCards[3]) {
                        players[plrIdSwitch].openingCards[3] = 1;
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, flipcard, 0);
                    }
                    else if (isMouseInside(cardRect5, mouseX, mouseY) && !players[plrIdSwitch].openingCards[4]) {
                        players[plrIdSwitch].openingCards[4] = 1;
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, flipcard, 0);
                    }
                    else if (isMouseInside(flipAll_buttonRect, mouseX, mouseY)) {
                        players[plrIdSwitch].openingCards[0] = 1;
                        players[plrIdSwitch].openingCards[1] = 1;
                        players[plrIdSwitch].openingCards[2] = 1;
                        players[plrIdSwitch].openingCards[3] = 1;
                        players[plrIdSwitch].openingCards[4] = 1;
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, flipcard, 0);
                    }
                    else if (!(players[plrIdSwitch].openingCards[0] == 0 || players[plrIdSwitch].openingCards[1] == 0 || players[plrIdSwitch].openingCards[2] == 0 || players[plrIdSwitch].openingCards[3] == 0 || players[plrIdSwitch].openingCards[4] == 0) && isMouseInside(backButton_Rect, mouseX, mouseY)) {
                        if (isPlayingSoundFX) Mix_PlayChannel(-1, clickSound, 0);
                        if ((plrIdSwitch < playerAmount-1 && isPVP) || (plrIdSwitch < 0 && !isPVP)) plrIdSwitch++;
                        else {
                            for (auto& player : players) {
                                player.handStrength = evaluateHandStrength(player.cards);
                            }
                            std::vector<int> winners = findWinner(players);
                            for (int winner : winners) {
                                std::cout << "Winner: " << players[winner].username << " with hand " << players[winner].handStrength << "\n";
                                if (isPVP) {
                                    players[winner].wins++;
                                    players[winner].total_wins++;
                                }
                                else if (winner == 0) {
                                    players[0].wins++;
                                    players[0].total_wins++;
                                    break;
                                }
                            }
                            for (auto& player : players) {
                                if (isPVP) {
                                    player.total_battles++;
                                    player.battles ++;
                                    player.winrate = (player.wins / player.battles)*100;
                                    player.total_winrate = (player.total_wins / player.total_battles)*100;
                                    std::cout << player.username << ' ' << player.winrate << std::endl;
                                }
                                else if (!isPVP) {
                                    players[0].total_battles++;
                                    players[0].battles ++;
                                    players[0].winrate = (player.wins / player.battles)*100;
                                    players[0].total_winrate = (player.total_wins / player.total_battles)*100;
                                    std::cout << players[0].username << ' ' << players[0].winrate << std::endl;
                                    break;
                                }
                            }
                        } 
                    }
                }
            }
            else if (e.type == SDL_TEXTINPUT && inputActive) {
                if (status == 4 && inputText.length() <= 1 && *e.text.text >= '0' && *e.text.text <= '9') inputText += e.text.text;
                else if (status == 5 && inputUsernameText.length() <= 9 && (*e.text.text >= '0' && *e.text.text <= '9' || *e.text.text <= 'Z' && *e.text.text >= 'A' || *e.text.text <= 'z' && *e.text.text >= 'a')) inputUsernameText += e.text.text;
            } 
            else if (e.type == SDL_KEYDOWN && inputActive) {
                if (status == 4 && e.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty()) {
                    inputText.pop_back();
                }
                else if (status == 5 && e.key.keysym.sym == SDLK_BACKSPACE && !inputUsernameText.empty()) {
                    inputUsernameText.pop_back();
                }
            }
        }
        

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        if (status != 0) {
            if (status == 1 || status == 2 || status == 3 || status == 4 || status == 5) {
                SDL_RenderCopy(renderer, backButton_Texture, nullptr, isMouseInside(backButton_Rect, mouseX, mouseY) ? &backButton_hoverRect : &backButton_Rect);
            }
        }
        if (status == 0) {
            SDL_RenderCopy(renderer, title_Texture, nullptr, &title_Rect);
            SDL_RenderCopy(renderer, play_buttonTexture, nullptr, isMouseInside(play_buttonRect, mouseX, mouseY) ? &play_hoverRect : &play_buttonRect);
            SDL_RenderCopy(renderer, settings_buttonTexture, nullptr, isMouseInside(settings_buttonRect, mouseX, mouseY) ? &settings_hoverRect : &settings_buttonRect);
            SDL_RenderCopy(renderer, exit_buttonTexture, nullptr, isMouseInside(exit_buttonRect, mouseX, mouseY) ? &exit_hoverRect : &exit_buttonRect);
        }
        else if (status == 1) {
            SDL_RenderCopy(renderer, gameplaysTitle_Texture, nullptr, &gameplaysTitle_Rect);
            SDL_RenderCopy(renderer, Baccarat_buttonTexture, nullptr, isMouseInside(Baccarat_buttonRect, mouseX, mouseY) ? &Baccarat_hoverRect : &Baccarat_buttonRect);
            SDL_RenderCopy(renderer, Poker_buttonTexture, nullptr, isMouseInside(Poker_buttonRect, mouseX, mouseY) ? &Poker_hoverRect : &Poker_buttonRect);
            SDL_RenderCopy(renderer, BigTwo_buttonTexture, nullptr, isMouseInside(BigTwo_buttonRect, mouseX, mouseY) ? &BigTwo_hoverRect : &BigTwo_buttonRect);
        }
        else if (status == 2) {
            SDL_RenderCopy(renderer, settingBoard_Texture, nullptr, &settingBoard_Rect);

            if (isPlayingMusic) {
                SDL_RenderCopy(renderer, checkedMusic_buttonTexture, nullptr, &checkedMusic_buttonRect);
            } 
            else {
                SDL_RenderCopy(renderer, uncheckedMusic_buttonTexture, nullptr, &uncheckedMusic_buttonRect);
            }

            if (isPlayingSoundFX) {
                SDL_RenderCopy(renderer, checkedMusic_buttonTexture, nullptr, &checkedSoundFX_buttonRect);
            }
            else {
                SDL_RenderCopy(renderer, uncheckedMusic_buttonTexture, nullptr, &uncheckedSoundFX_buttonRect);
            }

        }
        else if (status == 3) {
            SDL_RenderCopy(renderer, gamemodeTitle_Texture, nullptr, &gamemodeTitle_Rect);
            SDL_RenderCopy(renderer, PVP_buttonTexture, nullptr, isMouseInside(PVP_buttonRect, mouseX, mouseY) ? &PVP_hoverRect : &PVP_buttonRect);
            SDL_RenderCopy(renderer, PVE_buttonTexture, nullptr, isMouseInside(PVE_buttonRect, mouseX, mouseY) ? &PVE_hoverRect : &PVE_buttonRect);
        }
        else if (status == 4) {
            if (wrongAnswer) SDL_RenderCopy(renderer, warnInvalidNums, nullptr, &warningInvalidNums_Rect);
            SDL_RenderCopy(renderer, enterPlayerNums_Texture, nullptr, &enterPlayerNums_Rect);
        // Vẽ ô nhập
            SDL_SetRenderDrawColor(renderer, inputActive ? activeBoxColor.r : inputBoxColor.r,
                                           inputActive ? activeBoxColor.g : inputBoxColor.g,
                                           inputActive ? activeBoxColor.b : inputBoxColor.b, 255);
            SDL_RenderFillRect(renderer, &inputBox);

        // Vẽ text trong ô nhập
            if (!inputText.empty()) {
                SDL_Rect textInputRect = {inputBox.x + 3, inputBox.y + 10, inputBox.w - 10, inputBox.h - 10};
                renderTextInput(renderer, main_font, inputText, textInputColor, textInputRect);
            }

            SDL_RenderCopy(renderer, confirm_Texture, nullptr, isMouseInside(confirm_Rect, mouseX, mouseY) ? &confirm_hoverRect : &confirm_Rect);
        }
        else if (status == 5) {
            if (wrongAnswer) SDL_RenderCopy(renderer, warnInvalidUsername, nullptr, &warningInvalidUsername_Rect);
            std::string tmp = "Enter player " + std::to_string(plrIdSwitch+1) + "'s username:";
            SDL_Texture* enterUsername_Text = renderText(tmp, SuperPixel_font, brownColor, renderer);
            SDL_RenderCopy(renderer, enterUsernameBoard_Texture, nullptr, &enterPlayerUsernameBoard_Rect);
            SDL_RenderCopy(renderer, enterUsername_Text, nullptr, &enterUsername_Rect);
        // Vẽ ô nhập
            SDL_SetRenderDrawColor(renderer, inputActive ? activeBoxColor.r : inputBoxColor.r,
                                           inputActive ? activeBoxColor.g : inputBoxColor.g,
                                           inputActive ? activeBoxColor.b : inputBoxColor.b, 255);
            SDL_RenderFillRect(renderer, &inputUsernameBox);

        // Vẽ text trong ô nhập
            if (!inputUsernameText.empty()) {
                short length = inputUsernameText.length();
                SDL_Rect textInputRect = {inputUsernameBox.x + 5, inputUsernameBox.y + 10, inputUsernameBox.w - 10, inputUsernameBox.h - 10};
                SDL_Rect text_Rect = {textInputRect.x + (180-length*17), textInputRect.y, 28 + length*35, 55}; 
                renderTextInput(renderer, SuperPixel_font, inputUsernameText, textInputColor, text_Rect);
            }

            SDL_RenderCopy(renderer, confirm_Texture, nullptr, isMouseInside(confirm_Rect, mouseX, mouseY) ? &confirm_hoverRect : &confirm_Rect);
        }
        else if (status == 6) {
            if (isPlayingMusic) {
                Mix_HaltMusic();
                isPlayingMusic = false;
                Mix_PlayMusic(PokerSound, -1);
            }
            if (!Load) {
                Poker(players, playerAmount);
                Load = true;
            }
            else {
                SDL_RenderCopy(renderer, yourCards_Texture, nullptr, &yourCardsRect);
                SDL_RenderCopy(renderer, (players[plrIdSwitch].openingCards[0] != 0) ? cardTextures[players[plrIdSwitch].card_id[0]] : cardTextures[52], nullptr, &cardRect1);
                SDL_RenderCopy(renderer, (players[plrIdSwitch].openingCards[1] != 0) ? cardTextures[players[plrIdSwitch].card_id[1]] : cardTextures[52], nullptr, &cardRect2);
                SDL_RenderCopy(renderer, (players[plrIdSwitch].openingCards[2] != 0) ? cardTextures[players[plrIdSwitch].card_id[2]] : cardTextures[52], nullptr, &cardRect3);
                SDL_RenderCopy(renderer, (players[plrIdSwitch].openingCards[3] != 0) ? cardTextures[players[plrIdSwitch].card_id[3]] : cardTextures[52], nullptr, &cardRect4);
                SDL_RenderCopy(renderer, (players[plrIdSwitch].openingCards[4] != 0) ? cardTextures[players[plrIdSwitch].card_id[4]] : cardTextures[52], nullptr, &cardRect5);
                if (players[plrIdSwitch].openingCards[0] == 0 || players[plrIdSwitch].openingCards[1] == 0 || players[plrIdSwitch].openingCards[2] == 0 || players[plrIdSwitch].openingCards[3] == 0 || players[plrIdSwitch].openingCards[4] == 0)
                    SDL_RenderCopy(renderer, flipAll_Texture, nullptr, isMouseInside(flipAll_buttonRect, mouseX, mouseY) ? &flipAll_hoverRect : &flipAll_buttonRect);
                else 
                    SDL_RenderCopy(renderer, next_Texture, nullptr, isMouseInside(next_buttonRect, mouseX, mouseY) ? &backButton_hoverRect : &backButton_Rect);
            }
            
        }
        SDL_RenderPresent(renderer);

        SDL_Delay(17);
    }
    

    close(window, renderer, backgroundTexture, hoverSound, nullptr, 1); //giải phóng memory
    close(window, renderer, play_buttonTexture, nullptr, nullptr, 2);
    close(window, renderer, settings_buttonTexture, nullptr, nullptr, 2);
    close(window, renderer, exit_buttonTexture, nullptr, nullptr, 2);
    close(window, renderer, title_Texture, nullptr, nullptr, 2);
    return 0;
}
