#include <SDL.h>
#include <SDL_image.h>

#include "classes.hpp"

Player::Player() {
    x = 0;
    y = 0;
    texture = NULL;
    light_radius = 5;
}

Player::~Player() {
    x = 0;
    y = 0;
    texture = NULL;
}