// utils.cpp
#include "utils.hpp"
#include "Character.hpp"
#include <iostream>

//load texture from file
SDL_Texture* loadTexture(const char* path) {
    SDL_Texture* newTexture = IMG_LoadTexture(renderer, path);
    if (newTexture == nullptr) {
        std::cerr << "Failed to load texture: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    return newTexture;
}

bool checkCollision(const SDL_FRect& rect1, const SDL_FRect& rect2) {
    return (rect1.x < rect2.x + rect2.w &&
            rect1.x + rect1.w > rect2.x &&
            rect1.y < rect2.y + rect2.h &&
            rect1.y + rect1.h > rect2.y);
}

bool checkGroundCollision(const SDL_FRect& player, const SDL_FRect& ground, float y_velocity) {
    return (player.x + player.w > ground.x &&
            player.x < ground.x + ground.w &&
            player.y + player.h > ground.y &&
            player.y + player.h - player.h * 0.5f < ground.y + ground.h && 
            y_velocity > 0 &&
            player.y + player.h - 5 < ground.y);
}

bool checkWallCollision(const SDL_FRect& player, const SDL_FRect& wall) {
    return (player.x + player.w - 2 > wall.x && 
            player.x + 2 < wall.x + wall.w &&   
            player.y + player.h - 15 > wall.y && 
            player.y + 10 < wall.y + wall.h);  
}

bool checkCeilingCollision(const SDL_FRect& player, const SDL_FRect& ceiling, float y_velocity) {
    return (player.x < ceiling.x + ceiling.w &&
            player.x + player.w > ceiling.x &&
            player.y < ceiling.y + ceiling.h &&
            player.y + player.h > ceiling.y &&
            y_velocity < 0); 
}



