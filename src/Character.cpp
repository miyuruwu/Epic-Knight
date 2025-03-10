// Character.cpp
#include "Character.hpp"
#include "Utils.hpp"
#include "Game.hpp"

Character::Character()
    : idle("res/images/character_idle.png", 0, 0, 64, 16),
      run("res/images/character_run.png", 0, 0, 96, 16),
      attack("res/images/character_attack.png", 0, 0, 128, 16) {
    x = SCREEN_WIDTH / 2;
    y = 544;
    isMoving = false;
    isAttacking = false;
    facingRight = true;
    idle_frame = 0;
    run_frame = 0;
    attack_frame = 0;
    animationTimer = 0.0f;
    boundingBox = {x, y, 32, 64};
}

void Character::update(float dt, const Uint8* state) {
    isMoving = false;
        isAttacking = state[SDL_SCANCODE_SPACE];

        if (isAttacking) return;

        float ms = 200.0f * dt;

        if (state[SDL_SCANCODE_LEFT]) {
            x -= ms;
            facingRight = false;
            isMoving = true;
        }
        if (state[SDL_SCANCODE_RIGHT]) {
            x += ms;
            facingRight = true;
            isMoving = true;
        }

        if (x < 0) x = 0;
        else if (x > SCREEN_WIDTH - 32) x = SCREEN_WIDTH - 32;

        boundingBox.x = x;
        boundingBox.y = y;
}

void Character::draw() {
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
        float draw_x = x;
        if (isAttacking && !facingRight) {
            draw_x -= (112 - 32);
        }

        if (isAttacking) {
            SDL_Rect srcRect_attack[4] = {
                {3, 0, 28, 16},
                {34, 0, 28, 16},
                {65, 0, 28, 16},
                {97, 0, 28, 16}
            };
            attack.draw(draw_x, y, 112, 64, &srcRect_attack[attack_frame], flip);
        } else if (isMoving) {
            SDL_Rect srcRect_run[6];
            for (int i = 0; i < 6; i++) {
                srcRect_run[i] = {i * 16, 0, 13, 16};
            }
            run.draw(x, y, 32, 64, &srcRect_run[run_frame], flip);
        } else {
            SDL_Rect srcRect_idle[4];
            for (int i = 0; i < 4; i++) {
                srcRect_idle[i] = {2 + i * 16, 0, 11, 16};
            }
            idle.draw(x, y, 32, 64, &srcRect_idle[idle_frame], flip);
        }
}

void Character::updateAnimation(float dt) {
    animationTimer += dt;
    if (animationTimer > 0.1f) {
        idle_frame = (idle_frame + 1) % 4;
        run_frame = (run_frame + 1) % 6;
        attack_frame = (attack_frame + 1) % 4;
        animationTimer = 0.0f;
    }
}

SDL_FRect Character::getAttackBoundingBox() const {
    if(isAttacking) {
        float attackWidth = 112;
        float attackHeight = 64;
        float attackX = facingRight ? x : x - (attackWidth-32);
        return {attackX, y, attackWidth, attackHeight};
    }
    return {0,0,0,0};
}