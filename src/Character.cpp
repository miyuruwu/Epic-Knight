// Character.cpp
#include "Character.hpp"
#include "Utils.hpp"
#include "Game.hpp"
#include <iostream>

Character::Character()
    : idle("res/images/character_idle.png", 0, 0, 64, 16),
      run("res/images/character_run.png", 0, 0, 96, 16),
      jump("res/images/character_jump.png", 0, 0, 48, 16),
      attack("res/images/character_attack.png", 0, 0, 128, 16) {
    x = SCREEN_WIDTH / 2;
    y = 544;
    isMoving = false;
    isAttacking = false;
    facingRight = true;
    isJumping = false;
    canDoubleJump = false;
    jumpKeyReleased = true;
    idle_frame = 0;
    run_frame = 0;
    attack_frame = 0;
    jump_frame = 0;
    animationTimer = 0.0f;
    boundingBox = {x, y, 32, 64};

    y_velocity = 0.0f;
    gravity = 500.0f; 
}

void Character::update(float dt, const Uint8* state) {
    isMoving = false;
    bool movingLeft = false, movingRight = false;
    isAttacking = state[SDL_SCANCODE_SPACE];

    if (isAttacking) return;

    previousX = x;
    previousY = y;

    float ms = 200.0f * dt;

    if (state[SDL_SCANCODE_LEFT]) {
        x -= ms;
        facingRight = false;
        isMoving = true;
        movingLeft = true;
    }
    if (state[SDL_SCANCODE_RIGHT]) {
        x += ms;
        facingRight = true;
        isMoving = true;
        movingRight = true;
    }

    if (state[SDL_SCANCODE_UP]) {
        if (jumpKeyReleased) { 
            if (!isJumping) { 
                y_velocity = -400.0f;
                isJumping = true;
                canDoubleJump = true; 
            } 
            else if (canDoubleJump) { 
                y_velocity = -350.0f;
                canDoubleJump = false;
            }
            jumpKeyReleased = false;
        }
    } else {
        jumpKeyReleased = true;
    }

    y_velocity += gravity * dt;
    y += y_velocity * dt;

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

    if (onGround && y_velocity > 0) {
        y = closestGround.y - boundingBox.h;
        y_velocity = 0.0f;
        isJumping = false;
        canDoubleJump = true;
    } else {
        isJumping = true;
    }

    if (hitWall) {  
        if (movingLeft) {  
            x = previousX;  
        }  
        if (movingRight) {  
            x = previousX; 
        }  
        if (y_velocity > 0) {  
            y += y_velocity * dt;
        }
    }
    
    
    if (hitCeiling) {
        y_velocity = 0.0f;
        y = previousY;
    }

    if (hitWall) {
        std::cout << "Wall Collision Detected at X: " << x << " | Falling: " << (y_velocity > 0) << std::endl;
    }
    
    
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
        idle_frame = (idle_frame + 1) % 4;
        run_frame = (run_frame + 1) % 6;
        attack_frame = (attack_frame + 1) % 4;
        jump_frame = (jump_frame + 1) % 3;
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
