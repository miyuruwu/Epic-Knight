#define SDL_MAIN_HANDLED
#include "json.hpp"
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<filesystem>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;
bool gameRunning = true, isMoving = false;
SDL_Event event;
float character_x = 704, character_y = 224;
const int TILE_SIZE = 32;

SDL_Texture* loadTexture(const char* path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (newTexture == nullptr) {
        std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    return newTexture;
}

class Entity {
public:
    SDL_Texture* texture;
    SDL_FRect rect;
    float x, y, w, h;
    Entity(const char* path, float x, float y, float w, float h) {
        texture = loadTexture(path);
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        rect = {x, y, w, h};
    }
    void draw(float x, float y, float w, float h, SDL_Rect* srcRect = nullptr, float angle = 0.0f) {
        rect.x = x;
        rect.y = y;
        rect.w = w;
        rect.h = h;
        SDL_RenderCopyExF(renderer, texture, srcRect, &rect, 0, NULL, SDL_FLIP_NONE);
    }
};

class Direction {
public:
    bool left, right, up, down;
    Direction() {
        left = right = up = down = false;
    }
};

void initialize() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
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

void close() {
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
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = false;
                startScreenActive = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                // Check if the mouse click is within the button's bounds
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                if (mouseX >= startButton.rect.x && mouseX <= startButton.rect.x + startButton.rect.w &&
                    mouseY >= startButton.rect.y && mouseY <= startButton.rect.y + startButton.rect.h) {
                    startScreenActive = false; // Exit start screen if the button is clicked
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the background
        background.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        // Draw the start button
        startButton.draw(SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT - 150, 200, 100);
        SDL_RenderPresent(renderer);
    }
}

std::vector<std::vector<int>> loadMap(const std::string& path) {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    std::cout << "Attempting to open map file: " << path << std::endl;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << path << std::endl;
        return {};
    }

    nlohmann::json mapData;
    file >> mapData;
    file.close();

    // Extract map dimensions
    int mapWidth = mapData["width"];
    int mapHeight = mapData["height"];
    std::cout << "Map dimensions: " << mapWidth << "x" << mapHeight << std::endl;

    // Extract tile data from the first layer
    std::vector<int> tileData = mapData["layers"][0]["data"];
    std::cout << "Tile data size: " << tileData.size() << std::endl;

    std::vector<std::vector<int>> map(mapHeight, std::vector<int>(mapWidth, 0));
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            map[y][x] = tileData[y * mapWidth + x];
        }
    }

    return map;
}

void renderMap(SDL_Renderer* renderer, const std::vector<std::vector<int>>& map, SDL_Texture* tilesetTexture, int tileSize) {
    SDL_Rect srcRect_air = {0, 0, 0, 0}; // Air tile (no texture)
    SDL_Rect srcRect_20 = {215, 29, tileSize, tileSize}; // Tile ID 20
    SDL_Rect srcRect_32 = {221, 64, tileSize, tileSize}; // Tile ID 32
    SDL_Rect srcRect_8 = {219, 0, tileSize, tileSize};   // Tile ID 8

    for (int y = 0; y < map.size(); y++) {
        for (int x = 0; x < map[y].size(); x++) {
            int tileID = map[y][x];
            SDL_Rect srcRect;
            switch (tileID) {
                case 0: // Air tile
                    srcRect = srcRect_air;
                    break;
                case 20: // Tile ID 20
                    srcRect = srcRect_20;
                    break;
                case 32: // Tile ID 32
                    srcRect = srcRect_32;
                    break;
                case 8: // Tile ID 8
                    srcRect = srcRect_8;
                    break;
                default:
                    srcRect = srcRect_air;
                    break;
            }

            // Define the destination rectangle (where to render on the screen)
            SDL_FRect destRect = {x * tileSize, y * tileSize, tileSize, tileSize};
            // Render the tile (if it's not an air tile)
            if (tileID != 0) {
                //std::cout << "Rendering tile ID " << tileID << " at (" << x << ", " << y << ")" << std::endl;
                SDL_RenderCopyF(renderer, tilesetTexture, &srcRect, &destRect);
            }
        }
    }
}

Direction character_direction;

void update() {
    // Update game logic here
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT]) {
        character_x -= 5; // Move left
        character_direction.left = true;
        isMoving = true;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        character_x += 5; // Move right
        character_direction.right = true;
        isMoving = true;
    }
    if (state[SDL_SCANCODE_UP]) {
        character_y -= 5; // Move up
        character_direction.up = true;
    }
    if (state[SDL_SCANCODE_DOWN]) {
        character_y += 5; // Move down
        character_direction.down = true;
    }
}

float DirectionToAngle(Direction direction) {
    if (direction.left) {
        return 180.0f;
    } else if (direction.right) {
        return 0.0f;
    } else if (direction.up) {
        return 270.0f;
    } else if (direction.down) {
        return 90.0f;
    } else {
        return 0.0f;
    }
}

void drawGameScreen() {
    // Load the map
    std::vector<std::vector<int>> map = loadMap("res/tiles/map1..tmj");

    // Load the tileset
    SDL_Texture* tilesetTexture = loadTexture("res/images/tileset_32x32.png");
    if (!tilesetTexture) {
        std::cerr << "Failed to load tileset texture." << std::endl;
        return;
    }

    // Define tile properties
    const int TILE_SIZE = 32; // Size of each tile in pixels

    // Load character entity
    Entity character_idle("res/images/herochar_idle_anim_strip_4.png", 0, 0, 64, 16);
    SDL_Rect character_srcRect_idle[4];
    for (int i = 0; i < 4; i++) {
        character_srcRect_idle[i] = {2 + i * 16, 0, 11, 16};
    }
    Entity character_run("res/images/herochar_run_anim_strip_6.png", 0, 0, 96, 16);
    SDL_Rect character_srcRect_run[6];
    for (int i = 0; i < 6; i++) {
        character_srcRect_run[i] = {i * 16, 0, 13, 16};
    }

    /* Find the platform position
    int platform_x = -1;
    int platform_y = -1;
    for (int y = 0; y < map.size(); y++) {
        for (int x = 0; x < map[y].size(); x++) {
            if (map[y][x] == 8) {
                platform_x = x * TILE_SIZE;
                platform_y = y * TILE_SIZE;
                break;
            }
        }
        if (platform_x != -1 && platform_y != -1) {
            break;
        }
    }

    // Adjust character position to stand on the platform
    character_x = platform_x;
    character_y = platform_y - 32; // Adjust the y position to place the character on top of the platform*/

    // Load background entities
    Entity background("res/images/background.png", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    Entity bg_0("res/images/bg_0.png", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    Entity bg_1("res/images/bg_1.png", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    Entity bg_2("res/images/bg_2.png", 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    int idle_frame = 0, run_frame = 0;
    Uint32 startTime = SDL_GetTicks();

    while (gameRunning) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = false;
            } else {
                update();
            }
        }

        // Update the frame for animation
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - startTime > 100) { // Change frame every 100 ms
            idle_frame = (idle_frame + 1) % 4;
            run_frame = (run_frame + 1) % 6;
            startTime = currentTime;
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw background layers
        background.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        bg_0.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        bg_1.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        bg_2.draw(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // Draw the map
        renderMap(renderer, map, tilesetTexture, TILE_SIZE);

        // Draw the character
        float angle = DirectionToAngle(character_direction);
        if(((SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LEFT] &&character_direction.left) || (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_RIGHT] && character_direction.right)) && isMoving) {
            character_run.draw(character_x, character_y, 32, 64, &character_srcRect_run[run_frame], angle);
        }
        else {
            character_idle.draw(character_x, character_y, 32, 64, &character_srcRect_idle[idle_frame], angle);
        }
        // Present the renderer
        SDL_RenderPresent(renderer);
    }

    // Clean up textures
    SDL_DestroyTexture(tilesetTexture);
}

int main(int argc, char* argv[]) {
    initialize();
    drawStartScreen();
    drawGameScreen();
    close();
    return 0;
}