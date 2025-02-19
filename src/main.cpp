#define SDL_MAIN_USE_CALLBACKS
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>
#include<SDL3/SDL.h>
#include<iostream>
#include "entity.h"
#include "player.h"
#define RENDER_ENTITIES(entities, entities_count, renderer) for(int i = 0; i < entities_count; i++) {entities[i].render(renderer);}
#define HANDLE_EVENTS_ENTITIES(entities, entities_count, event) for(int i = 0; i < entities_count; i++) {entities[i].handle_events(event);}
#define QUIT_ENTITIES(entities, entities_count) for(int i = 0; i < entities_count; i++) {entities[i].quit();}
#define UPDATE_ENTITIES(entities, entities_count) for(int i = 0; i < entities_count; i++) {entities[i].update();}
using namespace std;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Entity entities[MAX_ENTITIES];
int entities_count = 0;


void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    QUIT_ENTITIES(entities,entities_count);
    if(renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if(window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    SDL_Quit();
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    HANDLE_EVENTS_ENTITIES(entities, entities_count, event);
    if(event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

void update() {
    UPDATE_ENTITIES(entities,entities_count);
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderClear(renderer);
    RENDER_ENTITIES(entities, entities_count,renderer);
    SDL_RenderPresent(renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    update();
    render();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cerr << "Có lỗi trong quá trình khởi tạo: " << SDL_GetError() << endl;
        return SDL_APP_FAILURE;
    }

    window = SDL_CreateWindow("Me may beo", 800, 600, 0);
    if (!window) {
        cerr << "Có lỗi trong quá trình khởi tạo: " << SDL_GetError() << endl;
        return SDL_APP_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        cerr << "Có lỗi trong quá trình khởi tạo: " << SDL_GetError() << endl;
        return SDL_APP_FAILURE;
    }
    init_player(renderer);
    entities[entities_count++] = init_player(renderer);

    return SDL_APP_CONTINUE;
}