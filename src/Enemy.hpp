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
    int health;
    bool isEnlarged;

    Enemy(const char* path, float x, float y, float speed);
    void update(float dt, int score);
    void draw();
    void updateAnimation(float dt);
    void takeDamage();
};

class Spike {
public:
    Entity spike;
    float x, y;
    int spike_frame;
    Uint32 spawn_time;
    Uint32 reverseStartTime;
    bool reversing;
    int SPIKE_LIFETIME;
    SDL_FRect boundingBox;

    Spike(const char* path, float x, float y);
    void draw();
    
};

#endif // ENEMY_HPP