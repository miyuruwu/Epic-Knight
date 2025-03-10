// Enemy.hpp
#ifndef ENEMY_HPP
#define ENEMY_HPP

#include "Entity.hpp"
#include <SDL2/SDL.h>

class Enemy {
public:
    Entity run;
    float x, y;
    bool isActive;
    int run_frame;
    float animationTimer;
    float speed;
    SDL_FRect boundingBox;

    Enemy(const char* path, float x, float y, float speed);
    void update(float dt);
    void draw();
    void updateAnimation(float dt);
    void kill();
};

#endif // ENEMY_HPP