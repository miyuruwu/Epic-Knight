#define SDL_MAIN_USE_CALLBACKS
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>
#include<SDL3/SDL.h>
#include<iostream>
using namespace std;

void quit() {

}

void handle_event() {

}

void update() {

}

void render() {
    // vẽ nhân vật
    SDL_FRect sprite_frame = {18,15,15,18};
    SDL_FRect player_position = {250,250,15,18};
    SDL_RenderTexture(renderer, player_texture, &sprite_frame, &player_position);
    SDL_SetTextureScaleMode(player_texture, SDL_SCALEMODE_NEAREST);
}

void init_player() {

}