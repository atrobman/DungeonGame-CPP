#ifndef TEXTURES_H
#define TEXTURES_H

#include <SDL.h>

enum TEXTURES {
    x_MISSING,
    x_FLOOR,
    x_WALL,
    x_DOOR,
    x_PLAYER,
    x_STAIRS_UP,
    x_STAIRS_DOWN,
    x_HIDDEN,
    x_DOOR_OPEN,
    x_KOBOLD,
    x_GOBLIN,
    x_HOBGOBLIN,
    x_ORC_BASIC,
    x_ORC_WARRIOR,
    x_ORC_WARLORD,

    NUM_TEXTURES //THIS MUST ALWAYS BE THE LAST ELEMENT OF THIS ENUM
};

extern SDL_Texture* gTextures[NUM_TEXTURES];

#endif