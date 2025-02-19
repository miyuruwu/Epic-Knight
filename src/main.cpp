#define SDL_MAIN_USE_CALLBACKS
#include<SDL3/SDL_main.h>
#include<SDL3_image/SDL_image.h>
#include<SDL3/SDL.h>
#include<iostream>
using namespace std;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* player_texture;

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
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
    if(event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}


// khởi tạo hình ảnh
void render() {
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    // vẽ nhân vật
    SDL_FRect sprite_frame = {18,15,15,18};
    SDL_FRect player_position = {250,250,15,18};
    SDL_RenderTexture(renderer, player_texture, &sprite_frame, &player_position);
    SDL_SetTextureScaleMode(player_texture, SDL_SCALEMODE_NEAREST);

    SDL_RenderPresent(renderer);
}

SDL_AppResult SDL_AppIterate(void *appstate) {
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

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        cerr << "Có lỗi trong quá trình khởi tạo: " << SDL_GetError() << endl;
        return SDL_APP_FAILURE;
    }
    
    const char path[] = "./char_spritesheet.png";
    player_texture = IMG_LoadTexture(renderer, path);

    return SDL_APP_CONTINUE;
}