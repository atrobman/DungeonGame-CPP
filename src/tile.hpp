#ifndef TILE_H
#define TILE_H

#include <SDL.h>
#include <SDL_image.h>

enum TileType {
    t_NONE,
    t_FLOOR,
    t_WALL,
    t_DOOR_CLOSED,
    t_STAIRS_UP,
    t_STAIRS_DOWN,
    t_DOOR_OPEN
};

class Tile {

public:
    Tile();
    ~Tile();

    SDL_Texture* texture;
    int weight;
    TileType type;
    bool unseen;
};

#endif