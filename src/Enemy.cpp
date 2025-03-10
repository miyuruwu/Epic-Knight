// Enemy.cpp
#include "Enemy.hpp"
#include "Utils.hpp"
#include "Game.hpp"

Enemy::Enemy(const char* path, float x, float y, float speed)
    : run(path, 0, 0, 96, 16) {
    this->x = x;
    this->y = y;
    this->speed = speed;
    isActive = true;
    run_frame = 0;
    animationTimer = 0.0f;
    boundingBox = {x, y, 32, 64};
}

void Enemy::update(float dt) {
    x += speed * dt; 
        if (x < -100 || x > SCREEN_WIDTH + 100) {
            isActive = false; 
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
        run.draw(x, y, 32, 64, &srcRect_run[run_frame], flip);
}

void Enemy::updateAnimation(float dt) {
    animationTimer += dt;
    if (animationTimer > 0.1f) {
        run_frame = (run_frame + 1) % 6;
        animationTimer = 0.0f;
    }
}

void Enemy::kill() {
    isActive = false;
}