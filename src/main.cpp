// main.cpp
#define SDL_MAIN_HANDLED
#include "Game.hpp"
#include <ctime>

int main(int argc, char* argv[]) {
    srand(static_cast<unsigned int>(time(0))); // seed generate random numbers
    initialize();
    drawStartScreen();
    drawGameScreen();
    close();
    return 0;
}