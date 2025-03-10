// Map.cpp
#include "Map.hpp"
#include "Utils.hpp"
#include <fstream>
#include <iostream>
#include "json.hpp"

std::vector<SDL_FRect> collisionTiles;

std::vector<std::vector<int>> loadMap(const std::string& path) {
    //std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    //std::cout << "Attempting to open map file: " << path << std::endl;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file: " << path << std::endl;
        return {};
    }

    nlohmann::json mapData;
    file >> mapData;
    file.close();

    int mapWidth = mapData["width"];
    int mapHeight = mapData["height"];
    //std::cout << "Map dimensions: " << mapWidth << "x" << mapHeight << std::endl;

    std::vector<int> tileData = mapData["layers"][0]["data"];
    //std::cout << "Tile data size: " << tileData.size() << std::endl;

    std::vector<std::vector<int>> map(mapHeight, std::vector<int>(mapWidth, 0));

    collisionTiles.clear(); 

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int tileID = tileData[y * mapWidth + x];
            map[y][x] = tileID;

            if (tileID == 20 || tileID == 32 || tileID == 8) { 
                SDL_FRect tileRect = {
                    static_cast<float>(x * TILE_SIZE),
                    static_cast<float>(y * TILE_SIZE),
                    static_cast<float>(TILE_SIZE),
                    static_cast<float>(TILE_SIZE)
                };
                collisionTiles.push_back(tileRect);
            }
        }
    }

    return map;
}

void renderMap(SDL_Renderer* renderer, const std::vector<std::vector<int>>& map, SDL_Texture* tilesetTexture, int tileSize) {
    SDL_Rect srcRect_air = {0, 0, 0, 0};
    SDL_Rect srcRect_20 = {215, 29, tileSize, tileSize};
    SDL_Rect srcRect_32 = {221, 64, tileSize, tileSize};
    SDL_Rect srcRect_8 = {219, 0, tileSize, tileSize};

    for (int y = 0; y < map.size(); y++) {
        for (int x = 0; x < map[y].size(); x++) {
            int tileID = map[y][x];
            SDL_Rect srcRect;
            switch (tileID) {
                case 0:
                    srcRect = srcRect_air;
                    break;
                case 20:
                    srcRect = srcRect_20;
                    break;
                case 32:
                    srcRect = srcRect_32;
                    break;
                case 8:
                    srcRect = srcRect_8;
                    break;
                default:
                    srcRect = srcRect_air;
                    break;
            }

            SDL_FRect destRect = {x * tileSize, y * tileSize, tileSize, tileSize};
            if (tileID != 0) {
                SDL_RenderCopyF(renderer, tilesetTexture, &srcRect, &destRect);
            }
        }
    }
}