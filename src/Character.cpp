// Character.cpp
#include "Character.hpp"
#include "Utils.hpp"
#include "Game.hpp"
#include <iostream>

Character::Character()
    : idle("res/images/character_idle.png", 0, 0, 64, 16),
      run("res/images/character_run.png", 0, 0, 96, 16),
      jump("res/images/character_jump.png", 0, 0, 48, 16),
      attack("res/images/character_attack.png", 0, 0, 128, 16),
      death("res/images/character_death.png", 0, 0, 128,16) {
    x = SCREEN_WIDTH / 2;
    y = 544;
    isMoving = false;
    isAttacking = false;
    facingRight = true;
    isJumping = false;
    jumpKeyReleased = true;
    isWallSliding = false;
    isDead = false;
    idle_frame = 0;
    run_frame = 0;
    attack_frame = 0;
    jump_frame = 0;
    death_frame = 0;
    animationTimer = 0.0f;
    boundingBox = {x, y, 32, 64};

    y_velocity = 0.0f;
    gravity = 500.0f; 
}

int cnt = 0;

void Character::update(float dt, const Uint8* state, bool paused) {
    if(isDead) {
        return;
    }
    std::cout << "Updating " << cnt++ << std::endl;

    if(paused) {
        std::cout << "game is paused" << std::endl;
        x = prev_x;
        y = prev_y;
        y_velocity = 0.0f;
        return;
    }

    isMoving = false;
    bool movingLeft = false, movingRight = false;
    
    if(state[SDL_SCANCODE_SPACE] && !isJumping) {
        isAttacking = true;
    } else {
        isAttacking = false;
    }

    previousX = x;
    previousY = y;

    float move_speed = 200.0f * dt;

    if (state[SDL_SCANCODE_LEFT]) {
        x -= move_speed;
        facingRight = false;
        isMoving = true;
        movingLeft = true;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        x += move_speed;
        facingRight = true;
        isMoving = true;
        movingRight = true;
    }

    if (state[SDL_SCANCODE_UP]) {
        if (jumpKeyReleased) { 
            if (!isJumping) { 
                y_velocity = -400.0f;
                isJumping = true;
            } 
            jumpKeyReleased = false;
        }
    } else {
        jumpKeyReleased = true;
    }

    y_velocity += gravity*dt;
    y += y_velocity*dt;

    if (x < 0) x = 0;
    else if (x > SCREEN_WIDTH - 32) x = SCREEN_WIDTH - 32;

    boundingBox.x = x;
    boundingBox.y = y;

    bool onGround = false;
    bool hitWall = false;
    bool hitCeiling = false;
    SDL_FRect closestGround = {0, 0, 0, 0};

    for (const auto& ground : grounds) {
        
        if (checkGroundCollision(boundingBox, ground, y_velocity)) {
            if (!onGround || ground.y < closestGround.y) {
                closestGround = ground; 
                onGround = true;
            }
        }

        if (checkWallCollision(boundingBox, ground)) {
            hitWall = true;
        }

        if (checkCeilingCollision(boundingBox, ground, y_velocity)) {
            hitCeiling = true;
        }
    }

    if (onGround) {
        if (y_velocity > 0) {
            y = closestGround.y - boundingBox.h;
            y_velocity = 0.0f;
            isJumping = false;
        }
    } else if (y_velocity > 500.0f) { 
        y_velocity = 0.0f;
        //std::cout << "Falling too fast, correcting position!" << std::endl;
    }

    if (hitWall) {  

        if (movingLeft) {  
            x = previousX + 3;
            y_velocity = 100.0f;
        }  
        if (movingRight) {  
            x = previousX - 3;
            y_velocity = 100.0f;
        }
    } else {
        isWallSliding = false;
    }

    if (hitCeiling) {
        y_velocity = 0.0f;
        y = previousY;
    }

    prev_x = x; prev_y = y;
}

void Character::draw() {
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    float draw_x = x;
    if (isAttacking && !facingRight) {
        draw_x -= (112 - 32);
    }

    if(isDead) {
        SDL_Rect srcRect_death[8] {
            {4,0,9,16},
            {20,0,9,16},
            {37,0,7,16},
            {52,0,9,16},
            {68,0,11,16},
            {84,0,11,16},
            {100,0,11,16},
            {115,0,11,16}
        };
        if(death_frame < 7) {
            death.draw(x,y,32,64, &srcRect_death[death_frame], flip);
        } else {
            death.draw(x,y,32,64, &srcRect_death[7], flip);
        }
        return;
    }

    if (isAttacking) {
        SDL_Rect srcRect_attack[4] = {
            {3, 0, 28, 16},
            {34, 0, 28, 16},
            {65, 0, 28, 16},
            {97, 0, 28, 16}
        };
        attack.draw(draw_x, y, 112, 64, &srcRect_attack[attack_frame], flip);
    } else if (isJumping) {
        SDL_Rect srcRect_jump[3];
        for(int i = 0; i < 3; i++) {
            srcRect_jump[i] = {2 + i * 16, 0, 12, 16};
        }
        jump.draw(x, y, 32, 64, &srcRect_jump[jump_frame],flip);
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
        if(isDead) {
            if(death_frame < 7) {
                death_frame++;
            } else {
                static Uint32 deathTimer = SDL_GetTicks();
                if(SDL_GetTicks() - deathTimer > 1500) {
                    gameRunning = false;
                }
            }
        } else {
            idle_frame = (idle_frame + 1) % 4;
            run_frame = (run_frame + 1) % 6;
            attack_frame = (attack_frame + 1) % 4;
            jump_frame = (jump_frame + 1) % 3;
        }
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

void Character::setGrounds(const std::vector<SDL_FRect>& groundsList) {
    grounds = groundsList;
}

void Character::character_die() {
    if(isDead) return;
    isDead = true;
    death_frame = 0;
}
