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
bool paused = false;
float prev_x = 0, prev_y = 0;
int score = 0;
Uint32 warningSignStartTime = 0;  
bool isWarningMessageVisible = false, spikeSpawned = false; 
Uint32 warningSignInterval = 15000; 
Uint32 warningSignDuration = 5000;

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

    window = SDL_CreateWindow("Epic Knight v2.0", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "Failed to initialize SDL_audio: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

void close() { // cleanup resources
    Mix_Quit();
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
    Entity background("res/images/start_background.jpeg", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
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
        startButton.draw(SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 150, 200, 100);
        SDL_RenderPresent(renderer);
    }
}

void resetGame() {
    gameRunning = true;
    paused = false;

    Character character;
    character.setGrounds(collisionTiles);
    std::vector<Enemy> enemies;

    Uint32 startTime = SDL_GetTicks();
    float spawnTimer = 0.0f;
    score = 0;

    warningSignStartTime = 0;
    isWarningMessageVisible = false;
    spikeSpawned = false;

    drawGameScreen();
}

void draw_score() {
    SDL_Color textColor = {255, 255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {20, 20, 150, 50}; 
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
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
    std::vector<Spike> spikes;
    std::vector<flyingEnemy> flyingEnemies;
    float spawnTimer = 0.0f, spawnFlyingTimer = 0.0f;
    const float spawnInterval = 2.0f; // spawn enemies every 2 seconds
    const float spawnFlyingInterval = 4.0f; // spawn flying enemies every 4 seconds

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
            else if (event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_ESCAPE) {
                    paused = !paused;
                    if(paused) {
                        draw_pause_screen();
                        startTime = SDL_GetTicks();
                    }
                }
            }
        }

        if(!character.isDead) {
            const Uint8* state = SDL_GetKeyboardState(nullptr);
            character.update(dt, state, paused);
        }

        character.updateAnimation(dt);

        spawnTimer += dt;
        spawnFlyingTimer += dt;
        if (spawnTimer >= spawnInterval) {
            spawnTimer = 0.0f;
            if (rand() % 2 == 0) {
                enemies.push_back(Enemy("res/images/enemy_run.png", -100, 544, 100)); // spawn left
            } else {
                enemies.push_back(Enemy("res/images/enemy_run.png", SCREEN_WIDTH + 100, 544, -100));  //spawn right
            }
        }
        if (spawnFlyingTimer >= spawnFlyingInterval) {
            spawnFlyingTimer = 0.0f;
            int minY = 250;
            int maxY = 500;
            int randomY = rand() % (maxY - minY + 1) + minY;
            if (rand() % 2 == 0) {
                flyingEnemies.push_back(flyingEnemy("res/images/flying_enemy.png", SCREEN_WIDTH + 100, randomY, -100));
            } else {
                flyingEnemies.push_back(flyingEnemy("res/images/flying_enemy.png", -100, randomY, 100));
            }
        }

        if (score >= 100) {

            Uint32 currentTime = SDL_GetTicks();
        
            if (warningSignStartTime == 0 || (currentTime - warningSignStartTime >= warningSignInterval)) {
                warningSignStartTime = currentTime;
                isWarningMessageVisible = true;
                spikeSpawned = false;
            }
        
            if (!spikeSpawned) {
                bool blinkOn = (((currentTime - warningSignStartTime) % 1000) < 500);
                isWarningMessageVisible = blinkOn;
            } else {
                isWarningMessageVisible = false;
            }

            if((currentTime - warningSignStartTime >= warningSignDuration) && !spikeSpawned) {
                //std::cout << "Spawning spike at: " << currentTime << std::endl;
                int numberOfSpikes = 8;
                for(int i = 0; i < numberOfSpikes; i++) {
                    float randomX = static_cast<float>(rand() % (SCREEN_WIDTH - 100) + 50);
                    spikes.push_back(Spike("res/images/trap_spike.png",randomX, 544));
                }
                spikeSpawned = true;
            }

            spikes.erase(std::remove_if(spikes.begin(), spikes.end(), [](const Spike& spike) {
                Uint32 now = SDL_GetTicks();
                return (now - spike.spawn_time) > spike.SPIKE_LIFETIME;
            }), spikes.end());

        }
        
        if(character.isAttacking) {
            SDL_FRect attackBox = character.getAttackBoundingBox();
            for(auto& enemy : enemies) {
                if(checkCollision(attackBox,enemy.boundingBox)) {
                    //std::cout << "enemy's health: " << enemy.health << std::endl;
                    enemy.takeDamage();
                    if(enemy.health <= 0) {
                        score += enemy.isEnlarged ? 30 : 10;
                    }
                }
            }
            for(auto& flyingEnemy : flyingEnemies) {
                if(checkCollision(attackBox, flyingEnemy.boundingBox)) {
                    flyingEnemy.kill();
                    score += 10;
                }
            }
        }

        for(auto& spike: spikes) {
            if(checkCollision(character.boundingBox,spike.boundingBox)) {
                character.character_die();
            }
        }

        for (auto& enemy : enemies) {
            enemy.update(dt, score);
            enemy.updateAnimation(dt);

            if(checkCollision(character.boundingBox,enemy.boundingBox)) {
                character.character_die();
            }
        }

        for(auto& flyingEnemy: flyingEnemies) {
            flyingEnemy.update(dt);
            flyingEnemy.updateAnimation(dt);
            if(checkCollision(character.boundingBox,flyingEnemy.boundingBox)) {
                character.character_die();
            }
        }

        flyingEnemies.erase(std::remove_if(flyingEnemies.begin(), flyingEnemies.end(),
            [](const flyingEnemy& flyingEnemy) { return !flyingEnemy.isActive; }), flyingEnemies.end());

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

        for(auto& spike: spikes) {
            //std::cout << "Drawing spikes at: " << spike.x << ' ' << spike.y << std::endl;
            if(!spike.spike.texture) {
                std::cerr << "error handling texture" << std::endl;
            }
            spike.draw();
        }

        for (auto& enemy : enemies) {
            enemy.draw();
        }

        for(auto& flyingEnemy: flyingEnemies) {
            flyingEnemy.draw();
        }

        draw_score();

        if (isWarningMessageVisible) {
            SDL_Color textColor = {255, 0, 0, 255};
            std::string warningText = "Watch your legs!!!";
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, warningText.c_str(), textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            SDL_Rect textRect = {SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT - 100, 400, 50};
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

            SDL_DestroyTexture(textTexture);
            SDL_FreeSurface(textSurface);
        }

        SDL_RenderPresent(renderer);

        if(!gameRunning && character.isDead) {
            draw_gameover_screen();
            return;
        } else if(!gameRunning && character.isDead) {
            return;
        }
    }

    SDL_DestroyTexture(tilesetTexture);
}

void draw_pause_screen() {
    bool menu_active = true;
    int selected_option = 0; // 0 = resume, 1 = restart, 2 = quit
    const char* options[] = {"Resume","Restart","Quit"};
    SDL_Color text_color = {255,255,255,255};

    while(menu_active) {
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                gameRunning = false;
                return;
            } else if (event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_DOWN) {
                    selected_option = (selected_option+1)%3; // cycle down options
                } else if (event.key.keysym.sym == SDLK_UP) {
                    selected_option = (selected_option -1 +3) % 3; // cycle up options
                } else if (event.key.keysym.sym == SDLK_RETURN) {
                    switch (selected_option)
                    {
                    case 0: // resume
                        paused = false;                      
                        return;
                    case 1: // restart
                        paused = false;
                        drawGameScreen();
                        return;
                    case 2: // quit
                        gameRunning = false;
                        return;
                    }
                } 
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
        SDL_RenderClear(renderer);

        for(int i = 0; i < 3; i++) {
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, options[i], selected_option == i ? SDL_Color{255, 0, 0, 255} : text_color);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

            SDL_Rect textRect = { SCREEN_WIDTH / 2 - 50, 300 + i * 50, 100, 40 };
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

            SDL_DestroyTexture(textTexture);
            SDL_FreeSurface(textSurface);
        }
        SDL_RenderPresent(renderer);
    }
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
                    resetGame();
                } else if (event.key.keysym.sym == SDLK_ESCAPE) { // Quit game
                    gameRunning = false;
                    gameOverActive = false;
                    return;
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