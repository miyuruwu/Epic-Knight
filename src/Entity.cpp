// Entity.cpp
#include "Entity.hpp"
#include "Utils.hpp"

Entity::Entity(const char* path, float x, float y, float w, float h) {
    texture = loadTexture(path);
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    rect = {x, y, w, h};
}

void Entity::draw(float x, float y, float w, float h, SDL_Rect* srcRect, SDL_RendererFlip flip) {
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_RenderCopyExF(renderer, texture, srcRect, &rect, 0, NULL, flip);
}