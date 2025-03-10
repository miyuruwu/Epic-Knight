// Character.hpp
#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "Entity.hpp"
#include <SDL2/SDL.h>

class Character {
public:
    Entity idle;
    Entity run;
    Entity attack;
    float x, y;
    bool isMoving, isAttacking, facingRight;
    int idle_frame, run_frame, attack_frame;
    float animationTimer;
    SDL_FRect boundingBox;

    Character();
    void update(float dt, const Uint8* state);
    void draw();
    void updateAnimation(float dt);
    SDL_FRect getAttackBoundingBox() const;
};

#endif // CHARACTER_HPP