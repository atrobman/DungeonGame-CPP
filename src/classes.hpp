#ifndef CLASSES_H
#define CLASSES_H

#define DUNGEON_WIDTH 80
#define DUNGEON_HEIGHT 80
#define TILE_SIZE 32
#define SCREEN_HEIGHT (1080 / 1)
#define SCREEN_WIDTH (1920 / 1)
#define NUMBER_ROOMS_MIN 10
#define NUMBER_ROOMS_MAX 20
#define MAX_CORRIDOR_LENGTH 45
#define MAX_ROOM_GEN_TRIES 10
#define RENDER_DISTANCE 35

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <vector>

class Enemy;
class Player;
class Game;
class RoomTemplate;
class EnemyTemplate;
class GroupTemplate;

enum ENEMY_TYPE {
    e_NONE,
    e_PLAYER, //This is an identifier for a prototype enemy used to represent the player
              //This is used exclusively for Initiative and Turn Order
    e_KOBOLD,
    e_GOBLIN,
    e_HOBGOBLIN,
    e_ORC_BASIC,
    e_ORC_WARRIOR,
    e_ORC_WARLORD
};

enum ENEMY_AI {

    ai_NONE,
    ai_RANDOM,
    ai_MELEE

};

#include "tile.hpp"
#include "label.hpp"
#include "textures.hpp"

class Enemy
{

public:
    Enemy();
    Enemy(ENEMY_TYPE type);
    ~Enemy();
    void turn(Game* game);

    ENEMY_AI aiType;
    ENEMY_TYPE etype;
    int health;
    int armor_class;
    int x;
    int y;
    unsigned long id;
    SDL_Texture* texture;

private:

};

#include "templates.hpp"

class Player
{

public:
    Player();
    ~Player();

    SDL_Texture* texture;
    int x;
    int y;
    int light_radius;
};

class Game
{
private:

    SDL_Texture* loadTexture( std::string path );
    
    SDL_Window* gWindow;
    SDL_Surface* gScreenSurface;
    TTF_Font* font;
    std::vector<RoomTemplate> roomTemplates;
    std::vector<EnemyTemplate> enemyTemplates;
    std::vector<GroupTemplate> groupTemplates;
    SDL_Renderer* gRenderer;
    Label test;
    int round;
    bool playerTurn;
    
public:

    Game();
    ~Game();

    bool init();
    bool loadMedia();
    void init_tilemap();
    void draw();
    void close();
    void move_player_to(int x, int y);
    void generate_dungeon(bool up);
    Tile getTile(int x, int y);
    void turn();

    Player player;
    int level;
    std::vector<std::vector<Tile>> tilemap;
    std::vector<Enemy> enemies;
};

#endif