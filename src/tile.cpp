#include <SDL.h>
#include <SDL_image.h>

#include "tile.hpp"

Tile::Tile() {
    texture = NULL;
    weight = 0;
    type = t_NONE;
    unseen = true;
}

Tile::~Tile() {
    texture = NULL;
    weight = 0;
    type = t_NONE;
    unseen = true;
}
