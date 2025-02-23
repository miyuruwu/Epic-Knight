#include<SDL3/SDL.h>
#include<SDL3_image/SDL_image.h>
#include<iostream>
using namespace std;

int main(int argc, char* argv[]) {
    if(SDL_Init(SDL_INIT_VIDEO) > 0) {
        cerr << "SDL_Init has failed. SDL_Error: " << SDL_GetError() << endl;
        return 1;
    }
    if(!(IMG_Init(IMG_INIT_PNG))) {
        cerr << "IMG_Init has failed. Error: " << SDL_GetError() << endl;
    }
    RenderWindow window("Epic Knight v1.0", 1280, 720);
}