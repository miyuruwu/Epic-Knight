#ifndef PLAYER_H
#define PLAYER_H
#include<SDL3/SDL.h>
#include<SDL3_image/SDL_image.h>
#include<iostream>
#include "entity.h"

Entity init_player(SDL_Renderer* renderer);

#endif