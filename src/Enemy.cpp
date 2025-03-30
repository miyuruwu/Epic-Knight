// Enemy.cpp
#include "Enemy.hpp"
#include "Utils.hpp"
#include "Game.hpp"
#include<SDL2/SDL.h>

Enemy::Enemy(const char* path, float x, float y, float speed)
    : run(path, 0, 0, 96, 16) {
    this->x = x;
    this->y = y;
    this->speed = speed;
    this->health = 1;
    this->isEnlarged = false;
    isActive = true;
    run_frame = 0;
    animationTimer = 0.0f;
    boundingBox = {x, y, 32, 64};
}

void Enemy::update(float dt, int score) {
    x += speed * dt; 
        if (x < -100 || x > SCREEN_WIDTH + 100) {
            isActive = false; 
        }

        if(score >= 200 && !isEnlarged) {
            isEnlarged = true;
            health = 5;
            boundingBox.w *= 2;
            boundingBox.h *= 2;
            y -= boundingBox.h/2;
        }

        boundingBox.x = x;
        boundingBox.y = y;
}

void Enemy::draw() {
    SDL_RendererFlip flip = (speed < 0) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_Rect srcRect_run[6] ={
            {3,0,12,16},
            {18,0,13,16},
            {35,0,12,16},
            {51,0,12,16},
            {67,0,12,16},
            {83,0,12,16},
        };

        int width = isEnlarged ? 64 : 32;
        int height = isEnlarged ? 128 : 64;
        run.draw(x, y, width, height, &srcRect_run[run_frame], flip);
}

void Enemy::updateAnimation(float dt) {
    animationTimer += dt;
    if (animationTimer > 0.1f) {
        run_frame = (run_frame + 1) % 6;
        animationTimer = 0.0f;
    }
}


void Enemy::takeDamage() {
    
    if(health > 0) {
        health--;
    }

    if(health <= 0) {
        isActive = false;
    }
}

Spike::Spike(const char* path, float x, float y)
    : spike(path, 0, 0, 112, 16) {
        this->x = x;
        this->y = y;
        spike_frame = 0;
        spawn_time = SDL_GetTicks();
        reverseStartTime = spawn_time + 3000;
        reversing = false;
        SPIKE_LIFETIME = 5000;
        boundingBox = {x,y,32,64};
}

void Spike::draw() {
    Uint32 currentTime = SDL_GetTicks();

    if(currentTime > reverseStartTime) {
        reversing = true;
    }

    SDL_RendererFlip flip = SDL_FLIP_VERTICAL;
    SDL_Rect srcRect_spike[7];
    for(int i = 0; i < 7; i++) {
        srcRect_spike[i] = {i*16, 0, 16, 16};
    }
    
    if (reversing) {
        spike_frame = 6 - ((currentTime - reverseStartTime) / 500) % 7;
    } else {
        spike_frame = ((currentTime - spawn_time) / 500) % 7;
    }

    if (spike_frame < 0) spike_frame = 0;

    int baseHeight = 64;
    int frameCount = 7;
    int heightPerFrame = baseHeight / frameCount; 
    int currentHeight = (spike_frame + 1) * heightPerFrame; 

    boundingBox.x = x;
    boundingBox.y = y + (baseHeight - currentHeight);
    boundingBox.w = 32;
    boundingBox.h = currentHeight; 
    spike.draw(x, y, 32, 64, &srcRect_spike[spike_frame], flip);
}

flyingEnemy::flyingEnemy(const char* path, float x, float y, float speed)
    : fly(path, 0, 0, 24, 8) {
    this->x = x;
    this->y = y;
    this->speed = speed;
    isActive = true;
    fly_frame = 0;
    animationTimer = 0.0f;
    boundingBox = {x, y, 32, 32};
}

void flyingEnemy::update(float dt) {
    x += speed * dt;
    if (x < -100 || x > SCREEN_WIDTH + 100) {
        isActive = false;
    }
    boundingBox.x = x;
    boundingBox.y = y;
}

void flyingEnemy::draw() {
    SDL_RendererFlip flip = (speed < 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_Rect srcRect_fly[3] = {
        {0,0,8,8},
        {8,0,8,8},
        {16,0,8,8}
    };

    fly.draw(x, y, 32, 32, &srcRect_fly[fly_frame], flip);
}

void flyingEnemy::updateAnimation(float dt) {
    animationTimer += dt;
    if (animationTimer > 0.1f) {
        fly_frame = (fly_frame + 1) % 3;
        animationTimer = 0.0f;
    }
}

void flyingEnemy::kill() {
    isActive = false;
}