#include<SDL3/SDL.h>
#include<SDL3_image/SDL_image.h>
#include<iostream>
#include "entity.h"
#include "player.h"
using namespace std;

static SDL_Texture* player_texture;
static SDL_FRect sprite_frame = {18,15,15,18};

struct Position {
    float x, y;
};

Position position0 = {0,0};
static SDL_FRect player_position = {position0.x,position0.y,15,18};

static void quit() {
    if (player_texture) {
        SDL_DestroyTexture(player_texture);
        player_texture = nullptr;
    }
}


static void handle_events(SDL_Event* event) {

}

static void update() {
    SDL_PumpEvents();
    const bool *keyboard_state = SDL_GetKeyboardState(nullptr);
    if(keyboard_state[SDL_SCANCODE_W]){
        position0.y -= 1;
    }
    if(keyboard_state[SDL_SCANCODE_S]){
        position0.y += 1;
    }
    if(keyboard_state[SDL_SCANCODE_A]){
        position0.x -= 1;
    }
    if(keyboard_state[SDL_SCANCODE_D]){
        position0.x += 1;
    }
    player_position.x = position0.x;
    player_position.y = position0.y;
}

static void render(SDL_Renderer* renderer) {
    SDL_RenderTexture(renderer, player_texture, &sprite_frame, &player_position);
}

Entity init_player(SDL_Renderer* renderer) {
    const char path[] = "./char_spritesheet.png";
    player_texture = IMG_LoadTexture(renderer, path);
    if(!player_texture) {
        cerr << "Không thể tải ảnh người chơi: " << SDL_GetError() << endl;
        Entity player = {quit, handle_events, update, render};
        return player;
    }
    SDL_SetTextureScaleMode(player_texture, SDL_SCALEMODE_NEAREST);
    Entity player = {quit, handle_events, update, render};

    return player;
}