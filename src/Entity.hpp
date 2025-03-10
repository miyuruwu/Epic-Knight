// Entity.hpp
#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Entity {
public:
    SDL_Texture* texture;
    SDL_FRect rect;
    float x, y, w, h;

    Entity(const char* path, float x, float y, float w, float h);
    void draw(float x, float y, float w, float h, SDL_Rect* srcRect = nullptr, SDL_RendererFlip flip = SDL_FLIP_NONE);
};

#endif // ENTITY_HPP