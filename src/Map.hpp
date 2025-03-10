// Map.hpp
#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#define TILE_SIZE 32

extern std::vector<SDL_FRect> collisionTiles;

std::vector<std::vector<int>> loadMap(const std::string& path);
void renderMap(SDL_Renderer* renderer, const std::vector<std::vector<int>>& map, SDL_Texture* tilesetTexture, int tileSize);

#endif // MAP_HPP