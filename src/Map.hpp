// Map.hpp
#ifndef MAP_HPP
#define MAP_HPP

#include <vector>
#include <string>
#include <SDL2/SDL.h>

std::vector<std::vector<int>> loadMap(const std::string& path);
void renderMap(SDL_Renderer* renderer, const std::vector<std::vector<int>>& map, SDL_Texture* tilesetTexture, int tileSize);

#endif // MAP_HPP