// Character.hpp
#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Entity.hpp"
#include <SDL2/SDL.h>
#include <vector>

class Character {
public:
    Entity idle;
    Entity run;
    Entity attack;
    Entity jump;
    Entity death;
    float x, y;
    bool isMoving, isAttacking, facingRight, isJumping, jumpKeyReleased, isWallSliding, isDead;
    int idle_frame, run_frame, attack_frame, jump_frame, death_frame;
    float animationTimer;
    SDL_FRect boundingBox;
    float y_velocity;
    float gravity;

    Character();
    void update(float dt, const Uint8* state);
    void draw();
    void updateAnimation(float dt);
    void setGrounds(const std::vector<SDL_FRect>& groundsList);
    void character_die();
    SDL_FRect getAttackBoundingBox() const;
private:
    std::vector<SDL_FRect> grounds;
    float previousX, previousY;
};

#endif // CHARACTER_HPP