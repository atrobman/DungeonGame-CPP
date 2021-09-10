#ifndef TEMPLATES_H
#define TEMPLATES_H

#include <string>
#include <vector>

#include "tile.hpp"
#include "enemy.hpp"

#define SAME 0
#define ENEMY_SPAWN_ATTEMPTS 100
#define ENEMY_SPAWN_RANGE 6

class TileProto {

public:
    TileProto() {
        tile = Tile();
        connection_point = false;
        has_enemy = false;
        enemy = Enemy();
        x = 0;
        y = 0;
    }

    ~TileProto() {
        tile = Tile();
        connection_point = false;
        has_enemy = false;
        enemy = Enemy();
        x = 0;
        y = 0;
    }

    Tile tile;
    bool connection_point;
    bool has_enemy;
    Enemy enemy;
    int x;
    int y;
};

class EnemyTemplate {

public:

    EnemyTemplate();
    ~EnemyTemplate();
    Enemy generate();

    std::string name;
    std::pair<int, int> health;
    std::pair<int, int> armor_class;
    ENEMY_TYPE etype;

};

typedef std::pair<std::pair<int, int>, EnemyTemplate> MONSTER_DEFINITION;

class GroupTemplate {

public:

    GroupTemplate();
    ~GroupTemplate();
    std::vector<Enemy> generate();

    std::string name;
    std::vector<MONSTER_DEFINITION> templates;
};

class RoomTemplate {

public:

    RoomTemplate();
    ~RoomTemplate();
    std::vector<std::vector<TileProto>> generate(std::vector<GroupTemplate> groupTemplates);

    std::string name;
    int w;
    int h;
    std::vector<std::string> tilemap;
    int doors;
    std::string tval;
    std::vector<std::string> theme;

};

std::vector<RoomTemplate> parseRooms();
std::vector<EnemyTemplate> parseEnemies();
std::vector<GroupTemplate> parseGroups(std::vector<EnemyTemplate> enemyTemplates);

#endif