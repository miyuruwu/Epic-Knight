// Game.cpp
#include "Game.hpp"
#include "Character.hpp"
#include "Enemy.hpp"
#include "Map.hpp"
#include "Utils.hpp"
#include <iostream>
#include <algorithm>

//global variables
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;
bool gameRunning = true;
SDL_Event event;
TTF_Font* font = nullptr;

void initialize() {//initialize the screen
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    if (TTF_Init() != 0) {
        std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    font = TTF_OpenFont("res/fonts/Arial.ttf",24);
    if(!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    window = SDL_CreateWindow("Epic Knight v1.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == nullptr) {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        exit(1);
    }
}

void close() { // cleanup resources
    if(font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void drawStartScreen() { 
    Entity background("res/images/start_background.png", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    Entity startButton("res/images/start_button.png", SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT - 150, 200, 100);
    if (!background.texture || !startButton.texture) {
        std::cerr << "Failed to load textures." << std::endl;
        return;
    }
    bool startScreenActive = true;
    while (startScreenActive) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = false;
                startScreenActive = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (mouseX >= startButton.rect.x && mouseX <= startButton.rect.x + startButton.rect.w &&
                    mouseY >= startButton.rect.y && mouseY <= startButton.rect.y + startButton.rect.h) {
                    startScreenActive = false;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        background.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        startButton.draw(SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT - 150, 200, 100);
        SDL_RenderPresent(renderer);
    }
}

void draw_gameover_screen();

void drawGameScreen() {
    std::vector<std::vector<int>> map = loadMap("res/tiles/map1..tmj");
    SDL_Texture* tilesetTexture = loadTexture("res/images/tileset_32x32.png");
    if (!tilesetTexture) {
        std::cerr << "Failed to load tileset texture." << std::endl;
        return;
    }

    Entity background("res/images/background.png", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    Entity bg_0("res/images/bg_0.png", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    Entity bg_1("res/images/bg_1.png", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    Entity bg_2("res/images/bg_2.png", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    Character character;
    character.setGrounds(collisionTiles);
    std::vector<Enemy> enemies;
    float spawnTimer = 0.0f;
    const float spawnInterval = 2.0f; // spawn enemies every 2 seconds

    Uint32 startTime = SDL_GetTicks();
    while (gameRunning) {
        Uint32 frameStart = SDL_GetTicks();
        float dt = (frameStart - startTime) / 1000.0f;
        if (dt < 1.0f / 60.0f) {
            SDL_Delay((1.0f / 60.0f - dt) * 1000);
            dt = 1.0f / 60.0f;
        }
        startTime = frameStart;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = false;
            }
        }

        const Uint8* state = SDL_GetKeyboardState(nullptr);
        character.update(dt, state);
        character.updateAnimation(dt);

        spawnTimer += dt;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.0f;
            if (rand() % 2 == 0) {
                enemies.push_back(Enemy("res/images/enemy_run.png", -100, 544, 100)); // spawn left
            } else {
                enemies.push_back(Enemy("res/images/enemy_run.png", SCREEN_WIDTH + 100, 544, -100));  //spawn right
            }
        }

        if(character.isAttacking) {
            SDL_FRect attackBox = character.getAttackBoundingBox();
            for(auto& enemy : enemies) {
                if(checkCollision(attackBox,enemy.boundingBox)) {
                    enemy.kill();
                }
            }
        }

        for (auto& enemy : enemies) {
            enemy.update(dt);
            enemy.updateAnimation(dt);

            // check for collision
            if(checkCollision(character.boundingBox,enemy.boundingBox)) {
                draw_gameover_screen();
                return;
            }
        }

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& enemy) { return !enemy.isActive; }), enemies.end());

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        background.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        bg_0.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        bg_1.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        bg_2.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        renderMap(renderer, map, tilesetTexture, TILE_SIZE);
        character.draw();

        for (auto& enemy : enemies) {
            enemy.draw();
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(tilesetTexture);
}

void draw_gameover_screen() {
    Entity game_over("res/images/game_over_screen.jpeg", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    bool gameOverActive = true;
    while (gameOverActive) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = false;
                gameOverActive = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) {  // Restart game
                    gameOverActive = false;
                    drawGameScreen(); // Restart game loop
                } else if (event.key.keysym.sym == SDLK_ESCAPE) { // Quit game
                    gameRunning = false;
                    gameOverActive = false;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        game_over.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        SDL_Color textColor = {255, 255, 255, 255}; 
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, "Press Enter to restart, Esc to quit", textColor);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            if (textTexture) {
                SDL_Rect textRect = {SCREEN_WIDTH / 2 - 270, SCREEN_HEIGHT - 100, 500, 50}; // Position and size of the text
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
            SDL_FreeSurface(textSurface);
        }
        SDL_RenderPresent(renderer);
    }
}