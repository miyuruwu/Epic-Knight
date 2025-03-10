// Game.hpp
#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include <string>

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern bool gameRunning;
extern SDL_Event event;
extern const int TILE_SIZE;
extern TTF_Font* font;

void initialize();
void close();
void drawStartScreen();
void drawGameScreen();
void draw_gameover_screen();

#endif // GAME_HPP