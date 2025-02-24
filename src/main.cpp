#define SDL_MAIN_HANDLED
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<iostream>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
bool gameRunning = true;
SDL_Event event;

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

SDL_Texture* loadTexture(const char* path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (newTexture == nullptr) {
        std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    return newTexture;
}

void close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}

void drawStartScreen() {
    SDL_Texture* backgroundTexture = loadTexture("res/images/background.png");
    SDL_Texture* startButtonTexture = loadTexture("res/images/start_button.png");
    if (!backgroundTexture || !startButtonTexture) {
        std::cerr << "Failed to load textures." << std::endl;
        return;
    }
    // Draw the start button
    SDL_FRect buttonRect = {SCREEN_WIDTH / 2 - 75, SCREEN_HEIGHT - 150, 200, 100};
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

                if (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
                    mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h) {
                    startScreenActive = false; // Exit start screen if the button is clicked
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the background
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        // Draw the start button
        SDL_RenderCopyF(renderer, startButtonTexture, nullptr, &buttonRect);
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(startButtonTexture);
}

int main(int argc, char* argv[]) {
    initialize();
    drawStartScreen();
    while (gameRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                gameRunning = false;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }
    close();
    return 0;
}