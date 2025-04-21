// Game.hpp
#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <string>

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern bool gameRunning;
extern bool paused;
extern SDL_Event event;
extern const int TILE_SIZE;
extern TTF_Font* font;
extern float prev_x, prev_y;
extern Mix_Music* backgroundMusic;

void initialize();
void close();
void drawStartScreen();
void drawGameScreen();
void draw_pause_screen();
void draw_gameover_screen();
void resetGame();

#endif // GAME_HPP