// Utils.hpp
#ifndef UTILS_HPP
#define UTILS_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

extern SDL_Renderer* renderer;

SDL_Texture* loadTexture(const char* path);
bool checkCollision(const SDL_FRect& rect1, const SDL_FRect& rect2);
bool checkGroundCollision(const SDL_FRect& player, const SDL_FRect& ground, float y_velocity);
bool checkWallCollision(const SDL_FRect& player, const SDL_FRect& wall);
bool checkCeilingCollision(const SDL_FRect& player, const SDL_FRect& ceiling, float y_velocity);

#endif // UTILS_HPP