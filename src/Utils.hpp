// Utils.hpp
#ifndef UTILS_HPP
#define UTILS_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

extern SDL_Renderer* renderer;

SDL_Texture* loadTexture(const char* path);
bool checkCollision(const SDL_FRect& rect1, const SDL_FRect& rect2);

#endif // UTILS_HPP