#include<SDL3/SDL.h>
#include<SDL3_image/SDL_image.h>
#include<iostream>
#include "RenderWindow.hpp"

RenderWindow RenderWindow(const char* p_title, int p_w, int p_h) :window(nullptr), renderer(nullptr){
    window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOW);
    if(window == nullptr) {
        cerr << "Faild to init window: " << SDL_GetError() << endl;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDER_ACCERELATED);
}