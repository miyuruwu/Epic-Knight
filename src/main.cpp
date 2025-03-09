#define SDL_MAIN_HANDLED
#include "json.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <ctime>

//global variations
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;
bool gameRunning = true;
SDL_Event event;
const int TILE_SIZE = 32;
TTF_Font* font = nullptr;

//load texture from file
SDL_Texture* loadTexture(const char* path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (newTexture == nullptr) {
        std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    return newTexture;
}


//class to represent drawable object
class Entity {
public:
    SDL_Texture* texture;
    SDL_FRect rect;
    float x, y, w, h;
    Entity(const char* path, float x, float y, float w, float h) { //initialize position and size
        texture = loadTexture(path);
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        rect = {x, y, w, h};
    }
    // draw the texture on screen
    void draw(float x, float y, float w, float h, SDL_Rect* srcRect = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE) {
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        SDL_RenderCopyExF(renderer, texture, srcRect, &rect, 0, NULL, flip);
    }
};

class Direction { // track movement 
public:
    bool left, right;
    Direction() {
        left = right = false;
    }
};

bool checkCollision(const SDL_FRect& rect1, const SDL_FRect& rect2) {
    return (rect1.x < rect2.x + rect2.w &&
            rect1.x + rect1.w > rect2.x &&
            rect1.y < rect2.y + rect2.h &&
            rect1.y + rect1.h > rect2.y);
}

class Character {
public:
    Entity idle;
    Entity run;
    Entity attack;
    float x, y;
    bool isMoving, isAttacking, facingRight;
    int idle_frame, run_frame, attack_frame;
    float animationTimer;
    SDL_FRect boundingBox;

    Character()
        : idle("res/images/character_idle.png", 0, 0, 64, 16),
          run("res/images/character_run.png", 0, 0, 96, 16),
          attack("res/images/character_attack.png", 0, 0, 128, 16) {
        x = 0;
        y = 544;
        isMoving = false;
        isAttacking = false;
        facingRight = true;
        idle_frame = 0;
        run_frame = 0;
        attack_frame = 0;
        animationTimer = 0.0f;
        boundingBox = {x,y,32,64};
    }

    void update(float dt, const Uint8* state) { // update char's actions
        isMoving = false;
        isAttacking = state[SDL_SCANCODE_SPACE];

        if (isAttacking) return;

        float ms = 200.0f * dt;

        if (state[SDL_SCANCODE_LEFT]) {
            x -= ms;
            facingRight = false;
            isMoving = true;
        }
        if (state[SDL_SCANCODE_RIGHT]) {
            x += ms;
            facingRight = true;
            isMoving = true;
        }

        if (x < 0) x = 0;
        else if (x > SCREEN_WIDTH - 32) x = SCREEN_WIDTH - 32;

        boundingBox.x = x;
        boundingBox.y = y;
    }

    void draw() { // draw char
        SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        float draw_x = x;
        if (isAttacking && !facingRight) {
            draw_x -= (112 - 32);
        }

        if (isAttacking) {
            SDL_Rect srcRect_attack[4] = {
                {3, 0, 28, 16},
                {34, 0, 28, 16},
                {65, 0, 28, 16},
                {97, 0, 28, 16}
            };
            attack.draw(draw_x, y, 112, 64, &srcRect_attack[attack_frame], flip);
        } else if (isMoving) {
            SDL_Rect srcRect_run[6];
            for (int i = 0; i < 6; i++) {
                srcRect_run[i] = {i * 16, 0, 13, 16};
            }
            run.draw(x, y, 32, 64, &srcRect_run[run_frame], flip);
        } else {
            SDL_Rect srcRect_idle[4];
            for (int i = 0; i < 4; i++) {
                srcRect_idle[i] = {2 + i * 16, 0, 11, 16};
            }
            idle.draw(x, y, 32, 64, &srcRect_idle[idle_frame], flip);
        }
    }
    //update frames
    void updateAnimation(float dt) {
        animationTimer += dt;
        if (animationTimer > 0.1f) {
            idle_frame = (idle_frame + 1) % 4;
            run_frame = (run_frame + 1) % 6;
            attack_frame = (attack_frame + 1) % 4;
            animationTimer = 0.0f;
        }
    }
    SDL_FRect getAttackBoundingBox() const {
        if(isAttacking) {
            float attackWidth = 112;
            float attackHeight = 64;
            float attackX = facingRight ? x : x - (attackWidth-32);
            return {attackX, y, attackWidth, attackHeight};
        }
        return {0,0,0,0};
    }
};

class Enemy {
public:
    Entity run;
    float x, y;
    bool isActive;
    int run_frame;
    float animationTimer;
    float speed;
    SDL_FRect boundingBox;

    Enemy(const char* path, float x, float y, float speed)
        : run(path, 0, 0, 96, 16) {
        this->x = x;
        this->y = y;
        this->speed = speed;
        isActive = true;
        run_frame = 0;
        animationTimer = 0.0f;
        boundingBox = {x,y,32,64};
    }

    void update(float dt) {
        x += speed * dt; 
        if (x < -100 || x > SCREEN_WIDTH + 100) {
            isActive = false; 
        }

        boundingBox.x = x;
        boundingBox.y = y;
    }

    void draw() {
        SDL_RendererFlip flip = (speed < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_Rect srcRect_run[6] ={
            {3,0,12,16},
            {18,0,13,16},
            {35,0,12,16},
            {51,0,12,16},
            {67,0,12,16},
            {83,0,12,16},
        };
        run.draw(x, y, 32, 64, &srcRect_run[run_frame], flip);
    }

    void updateAnimation(float dt) {
        animationTimer += dt;
        if (animationTimer > 0.1f) {
            run_frame = (run_frame + 1) % 6;
            animationTimer = 0.0f;
        }
    }
    void kill() {
        isActive = false;
    }
};

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

std::vector<std::vector<int>> loadMap(const std::string& path) {
    //std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    //std::cout << "Attempting to open map file: " << path << std::endl;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << path << std::endl;
        return {};
    }

    nlohmann::json mapData;
    file >> mapData;
    file.close();

    int mapWidth = mapData["width"];
    int mapHeight = mapData["height"];
    //std::cout << "Map dimensions: " << mapWidth << "x" << mapHeight << std::endl;

    std::vector<int> tileData = mapData["layers"][0]["data"];
    //std::cout << "Tile data size: " << tileData.size() << std::endl;

    std::vector<std::vector<int>> map(mapHeight, std::vector<int>(mapWidth, 0));
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            map[y][x] = tileData[y * mapWidth + x];
        }
    }

    return map;
}

void renderMap(SDL_Renderer* renderer, const std::vector<std::vector<int>>& map, SDL_Texture* tilesetTexture, int tileSize) {
    SDL_Rect srcRect_air = {0, 0, 0, 0};
    SDL_Rect srcRect_20 = {215, 29, tileSize, tileSize};
    SDL_Rect srcRect_32 = {221, 64, tileSize, tileSize};
    SDL_Rect srcRect_8 = {219, 0, tileSize, tileSize};

    for (int y = 0; y < map.size(); y++) {
        for (int x = 0; x < map[y].size(); x++) {
            int tileID = map[y][x];
            SDL_Rect srcRect;
            switch (tileID) {
                case 0:
                    srcRect = srcRect_air;
                    break;
                case 20:
                    srcRect = srcRect_20;
                    break;
                case 32:
                    srcRect = srcRect_32;
                    break;
                case 8:
                    srcRect = srcRect_8;
                    break;
                default:
                    srcRect = srcRect_air;
                    break;
            }

            SDL_FRect destRect = {x * tileSize, y * tileSize, tileSize, tileSize};
            if (tileID != 0) {
                SDL_RenderCopyF(renderer, tilesetTexture, &srcRect, &destRect);
            }
        }
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


int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0))); // seed generate random numbers
    initialize();
    drawStartScreen();
    drawGameScreen();
    close();
    return 0;
}