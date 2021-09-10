#include <fstream>
#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <time.h>

#include "templates.hpp"
#include "tile.hpp"
#include "enemy.hpp"

// #define DEBUG_TEMPLATE_ROOM_MAIN
// #define DEBUG_TEMPLATE_ROOMS

// #define DEBUG_TEMPLATE_ENEMY
// #define DEBUG_TEMPLATE_ENEMY_MAIN

// #define DEBUG_TEMPLATE_GROUP

#define constrain(X, MIN, MAX) (X < MIN ? MIN : (X > MAX ? MAX : X))

std::vector<RoomTemplate> parseRooms() {

    std::vector<RoomTemplate> templates = std::vector<RoomTemplate>();

    std::ifstream infile("data/rooms.txt");
    std::string line;
    int i = 1;
    RoomTemplate room;
    std::vector<std::string> tilemap = std::vector<std::string>();
    int weight = 0;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);

        if (line[0] != '#' && line[0] != ' ' && line.size() > 1) {

            if (line.substr(0, 6).compare("name: ") == SAME) { //name directive
                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("name: ");
                #endif

                if (room.name.compare("") != SAME) { //since the name directive is ALWAYS first, lets check if this is the first one or not

                    if (weight <= 0 || tilemap.size() == 0 || room.w == 0 || room.h == 0 || room.tval.compare("") == SAME || room.doors == -1 || room.theme.size() == 0) {
                        //If the previous room has not yet been fully initialized, you fucked up
                        printf("ERROR AT LINE %d OF rooms.txt: NOT FULLY INITIALIZED. DUMPING PARTIAL TEMPLATE. ROOM: %s\n", i, room.name.c_str());
                        room = RoomTemplate();
                        tilemap = std::vector<std::string>();
                        weight = 0;

                    } else if ((size_t) room.w != tilemap.size() || (size_t) room.h != tilemap[0].size()) {
                        //If the listed width and height do not match
                        printf("ERROR AT LINE %d OF rooms.txt: PROVIDED TILEMAP FOR ROOM %s DOES NOT MATCH PROVIDED WIDTH AND HEIGHT.\n", i, room.name.c_str());
                        room = RoomTemplate();
                        tilemap = std::vector<std::string>();
                        weight = 0;

                    } else {
                        room.tilemap = tilemap;

                        for (int j = 0; j < weight; j ++) {
                            templates.push_back(room);
                        }

                        room = RoomTemplate();
                        tilemap = std::vector<std::string>();
                        weight = 0;
                    }

                    room.name = line.substr(6);
                } else {
                    room.name = line.substr(6);
                }

                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("%s\n", room.name.c_str());
                #endif

            } else if (line.substr(0, 8).compare("weight: ") == SAME) {

                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("weight: ");
                #endif

                if (weight != 0) {
                    printf("ERROR AT LINE %d OF rooms.txt: ROOM WEIGHT DEFINED TWICE. USING NEW WEIGHT\n", i);
                }

                std::string weight_str = line.substr(8);
                weight = std::stoi(weight_str);

                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("%d\n", weight);
                #endif

            } else if (line.substr(0, 6).compare("size: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("size: ");
                #endif

                if (room.w != 0 || room.h != 0) {
                    printf("ERROR AT LINE %d OF rooms.txt: ROOM SIZE DEFINED TWICE. USING NEW SIZE\n", i);
                }

                std::string h_str = line.substr(6, line.find('-'));
                std::string w_str = line.substr(line.find('-') + 1);

                room.w = std::stoi(w_str);
                room.h = std::stoi(h_str);
                
                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("%d-%d\n", room.w, room.h);
                #endif

            } else if (line.substr(0, 7).compare("doors: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("doors: ");
                #endif

                if (room.doors != -1) {
                    printf("ERROR AT LINE %d OF rooms.txt: DOOR CONFIG VALUE SET TWICE. USING NEW DOOR CONFIG VALUE\n", i);
                }

                std::string config_str = line.substr(7);
                room.doors = std::stoi(config_str);

                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("%d\n", room.doors);
                #endif
            } else if (line.substr(0, 6).compare("tval: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("tval: ");
                #endif

                if (room.tval.compare("") != SAME) {
                    printf("ERROR AT LINE %d OF rooms.txt: TVAL SET TWICE. USING NEW TVAL\n", i);
                }

                room.tval = line.substr(6);

                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("%s\n", room.tval.c_str());
                #endif
            } else if (line.substr(0, 7).compare("theme: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("theme:");
                #endif

                if (room.theme.size() > 0) {
                    printf("ERROR AT LINE %d OF rooms.txt: THEMES SET TWICE. USING NEW THEME CONFIG\n", i);
                }

                std::string line_ = line.substr(7);

                while (line_.find('-') != std::string::npos) {

                    std::string t = line_.substr(0, line_.find_first_of('-'));
                    line_ = line_.substr(line_.find_first_of('-') + 1);
                    room.theme.push_back(t);

                    #ifdef DEBUG_TEMPLATE_ROOMS
                        printf(" %s", t.c_str());
                    #endif
                }

                room.theme.push_back(line_);

                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf(" %s\n", line_.c_str());
                #endif
            } else if (line.substr(0, 2).compare("D:") == SAME) {

                #ifdef DEBUG_TEMPLATE_ROOMS
                    printf("%s\n", line.c_str());
                #endif

                tilemap.push_back(line.substr(2));
            }
        }

        i ++;
    }

    
    if (weight <= 0 || tilemap.size() == 0 || room.w == 0 || room.h == 0 || room.tval.compare("") == SAME || room.doors == -1 || room.theme.size() == 0) {
        //If the previous room has not yet been fully initialized, you fucked up
        printf("ERROR AT LINE %d OF rooms.txt: NOT FULLY INITIALIZED. DUMPING PARTIAL TEMPLATE. ROOM: %s\n", i, room.name.c_str());
        room = RoomTemplate();
        tilemap = std::vector<std::string>();
        weight = 0;

    } else if ((size_t) room.w != tilemap.size() || (size_t) room.h != tilemap[0].size()) {
        //If the listed width and height do not match
        printf("ERROR AT LINE %d OF rooms.txt: PROVIDED TILEMAP FOR ROOM %s DOES NOT MATCH PROVIDED WIDTH AND HEIGHT. DUMPING TEMPLATE.\n", i, room.name.c_str());
        room = RoomTemplate();
        tilemap = std::vector<std::string>();
        weight = 0;
        
    } else {
        room.tilemap = tilemap;
        templates.push_back(room);
        room = RoomTemplate();
        tilemap = std::vector<std::string>();
        weight = 0;
    }

    return templates;
}

std::vector<EnemyTemplate> parseEnemies() {

    std::vector<EnemyTemplate> templates = std::vector<EnemyTemplate>();

    std::ifstream infile("data/monsters.txt");
    std::string line;
    int i = 1;
    EnemyTemplate enemy;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);

        if (line[0] != '#' && line[0] != ' ' && line.size() > 1) {

            if (line.substr(0, 6).compare("name: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_ENEMY
                    printf("name: ");
                #endif

                if (enemy.name.compare("") != SAME) {

                    if (enemy.armor_class.first <= 0 || enemy.armor_class.first > enemy.armor_class.second || enemy.health.first <= 0 || enemy.health.first > enemy.health.second) {
                        printf("ERROR AT LINE %d OF monsters.txt: NOT FULLY INITIALIZED. DUMPING PARTIAL TEMPLATE. ENEMY: %s\n", i, enemy.name.c_str());
                        enemy = EnemyTemplate();
                    } else {
                        templates.push_back(enemy);

                        enemy = EnemyTemplate();
                    }

                    enemy.name = line.substr(6);

                } else {
                    enemy.name = line.substr(6);
                }

                #ifdef DEBUG_TEMPLATE_ENEMY
                    printf("%s\n", enemy.name.c_str());
                #endif                
            } else if (line.substr(0, 8).compare("health: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_ENEMY
                    printf("health: ");
                #endif

                if (enemy.health.first != 0) {
                    printf("ERROR AT LINE %d OF monsters.txt: ENEMY HEALTH DEFINED TWICE. USING NEW HEALTH\n", i);
                }

                std::string min_str = line.substr(8, line.find('-'));
                std::string max_str = line.substr(line.find('-') + 1);

                enemy.health.first = std::stoi(min_str);
                enemy.health.second = std::stoi(max_str);

                #ifdef DEBUG_TEMPLATE_ENEMY
                    printf("%d-%d\n", enemy.health.first, enemy.health.second);
                #endif
            } else if (line.substr(0, 4).compare("ac: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_ENEMY
                    printf("ac: ");
                #endif

                if (enemy.armor_class.first != 0) {
                    printf("ERROR AT LINE %d OF monsters.txt: ENEMY AC DEFINED TWICE. USING NEW AC\n", i);
                }

                std::string min_str = line.substr(4, line.find('-'));
                std::string max_str = line.substr(line.find('-') + 1);

                enemy.armor_class.first = std::stoi(min_str);
                enemy.armor_class.second = std::stoi(max_str);

                #ifdef DEBUG_TEMPLATE_ENEMY
                    printf("%d-%d\n", enemy.armor_class.first, enemy.armor_class.second);
                #endif
            } else if (line.substr(0, 7).compare("etype: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_ENEMY
                    printf("etype: ");
                #endif

                std::string etype = line.substr(7);

                if (etype.compare("e_NONE") == SAME) {
                    enemy.etype = e_NONE;
                } else if (etype.compare("e_KOBOLD") == SAME) {
                    enemy.etype = e_KOBOLD;
                } else if (etype.compare("e_GOBLIN") == SAME) {
                    enemy.etype = e_GOBLIN;
                } else if (etype.compare("e_HOBGOBLIN") == SAME) {
                    enemy.etype = e_HOBGOBLIN;
                } else if (etype.compare("e_ORC_BASIC") == SAME) {
                    enemy.etype = e_ORC_BASIC;
                } else if (etype.compare("e_ORC_WARRIOR") == SAME) {
                    enemy.etype = e_ORC_WARRIOR;
                } else if (etype.compare("e_ORC_WARLORD") == SAME) {
                    enemy.etype = e_ORC_WARLORD;
                } else {
                    printf("ERROR AT LINE %d OF monsters.txt: ENEMY ETYPE %s NOT FOUND\n", i, etype.c_str());
                    enemy.etype = e_NONE;
                }

                #ifdef DEBUG_TEMPLATE_ENEMY
                    printf("%s\n", etype.c_str());
                #endif
            }

        }

        i ++;
    }

    if (enemy.armor_class.first <= 0 || enemy.armor_class.first > enemy.armor_class.second || enemy.health.first <= 0 || enemy.health.first > enemy.health.second) {
        printf("ERROR AT LINE %d OF monsters.txt: NOT FULLY INITIALIZED. DUMPING PARTIAL TEMPLATE. ENEMY: %s\n", i, enemy.name.c_str());
        enemy = EnemyTemplate();
    } else {
        templates.push_back(enemy);

        enemy = EnemyTemplate();
    }

    return templates;
}

std::vector<GroupTemplate> parseGroups(std::vector<EnemyTemplate> enemyTemplates) {
    std::vector<GroupTemplate> templates = std::vector<GroupTemplate>();

    std::ifstream infile("data/monster_groups.txt");
    std::string line;
    int i = 1;
    GroupTemplate group;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);

        if (line[0] != '#' && line[0] != ' ' && line.size() > 1) {

            if (line.substr(0, 6).compare("name: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_GROUP
                    printf("name: ");
                #endif

                if (group.name.compare("") != SAME) {

                    if (group.templates.size() == 0) {
                        printf("ERROR AT LINE %d OF monster_groups.txt: DUMPING EMPTY GROUP. GROUP: %s\n", i, group.name.c_str());
                        group = GroupTemplate();
                    } else {
                        templates.push_back(group);

                        group = GroupTemplate();
                    }

                    group.name = line.substr(6);

                } else {
                    group.name = line.substr(6);
                }

                #ifdef DEBUG_TEMPLATE_GROUP
                    printf("%s\n", group.name.c_str());
                #endif                
            } else if (line.substr(0, 8).compare("m-name: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_GROUP
                    printf("m-name: ");
                #endif

                std::string m_name = line.substr(8);

                if (group.templates.size() > 0) {

                    if ( group.templates[group.templates.size() - 1].first.first == 0 ) {
                        printf("ERROR AT LINE %d OF monster_groups.txt: MONSTER DEFINITION LACKS AMOUNT. INITIALIZING TO 0. GROUP: %s\n", i, group.name.c_str());
                        group.templates[group.templates.size() - 1].first = std::make_pair(0, 0);
                    } else {

                        MONSTER_DEFINITION m_def;
                        bool template_found = false;

                        for (EnemyTemplate etp : enemyTemplates) {
                            if (m_name.compare(etp.name) == SAME) {
                                template_found = true;
                                m_def.second = etp;
                                break;
                            }
                        }

                        if (!template_found) {
                            printf("ERROR AT LINE %d OF monster_groups.txt: MONSTER TEMPLATE NOT FOUND. NAME: %s\n", i, m_name.c_str());
                            m_def.second = enemyTemplates[ e_NONE ];
                        }

                        group.templates.push_back(m_def);

                    }
                } else {

                    MONSTER_DEFINITION m_def;
                    bool template_found = false;

                    for (EnemyTemplate etp : enemyTemplates) {
                        if (m_name.compare(etp.name) == SAME) {
                            template_found = true;
                            m_def.second = etp;
                            break;
                        }
                    }

                    if (!template_found) {
                        printf("ERROR AT LINE %d OF monster_groups.txt: MONSTER TEMPLATE NOT FOUND. NAME: %s\n", i, m_name.c_str());
                        m_def.second = enemyTemplates[ e_NONE ];
                    }

                    group.templates.push_back(m_def);
                }

                #ifdef DEBUG_TEMPLATE_GROUP
                    printf("%s\n", m_name.c_str());
                #endif
            } else if (line.substr(0, 8).compare("amount: ") == SAME) {
                #ifdef DEBUG_TEMPLATE_GROUP
                    printf("amount: ");
                #endif

                std::string min_str = line.substr(8, line.find('-'));
                std::string max_str = line.substr(line.find('-') + 1);

                int min = std::stoi(min_str);
                int max = std::stoi(max_str);

                if (min > max) {
                    printf("ERROR AT LINE %d OF monster_groups.txt: AMOUNT MIN GREATER THAN MAX. SETTING MAX TO MIN+1\n", i);
                    max = min + 1;
                }
                
                if (group.templates.size() > 0) {
                    
                    if (group.templates[group.templates.size() - 1].first.first != 0) {
                        printf("ERROR AT LINE %d OF monster_groups.txt: AMOUNT DEFINED TWICE. USING NEW AMOUNT\n", i);
                    }

                    group.templates[group.templates.size() - 1].first = std::make_pair(min, max);

                } else {

                    MONSTER_DEFINITION m_def;
                    m_def.first.first = min;
                    m_def.first.second = max;

                    group.templates.push_back(m_def);
                }


                #ifdef DEBUG_TEMPLATE_GROUP
                    printf("%d-%d\n", min, max);
                #endif
            }
        }

        i ++;
    }

    if (group.templates.size() == 0) {
        printf("ERROR AT LINE %d OF monster_groups.txt: DUMPING EMPTY GROUP. GROUP: %s\n", i, group.name.c_str());
        group = GroupTemplate();
    } else {
        templates.push_back(group);

        group = GroupTemplate();
    }

    return templates;
}

//---------------------------------------------------

RoomTemplate::RoomTemplate() {
    name = "";
    w = 0;
    h = 0;
    tilemap = std::vector<std::string>();
    doors = -1;
    tval = "";
    theme = std::vector<std::string>();
}

RoomTemplate::~RoomTemplate() {
    name = "";
    w = 0;
    h = 0;
    tilemap = std::vector<std::string>();
    doors = -1;
    tval = "";
    theme = std::vector<std::string>();
}

EnemyTemplate::EnemyTemplate() {

    name = "";
    health = std::make_pair(0, 0);
    armor_class = std::make_pair(0, 0);

}

EnemyTemplate::~EnemyTemplate() {

    name = "";
    health = std::make_pair(0, 0);
    armor_class = std::make_pair(0, 0);

}

GroupTemplate::GroupTemplate() {

    name = "";
    templates = std::vector<MONSTER_DEFINITION>();

}

GroupTemplate::~GroupTemplate() {

    name = "";
    templates = std::vector<MONSTER_DEFINITION>();

}

//---------------------------------------------------

std::vector<std::vector<TileProto>> RoomTemplate::generate(std::vector<GroupTemplate> groupTemplates) {

    std::vector<std::vector<TileProto>> tilemapProto = std::vector<std::vector<TileProto>>();
    std::vector<std::pair<std::pair<int, int>, GroupTemplate>> groups = std::vector<std::pair<std::pair<int, int>, GroupTemplate>>();

    int tileset = rand() % 2;
    int door_config;
    if (doors > 0) {
        door_config = rand() % doors + 1;
    } else {
        door_config = 0;
    }

    for (int i = 0; i < w; i ++) {

        std::vector<TileProto> row = std::vector<TileProto>();

        for (int j = 0; j < h; j ++) {
            
            TileProto tp;
            Tile t = Tile();
            EnemyTemplate etp;

            if (tilemap[i][j] == '^') {

                t.type = t_NONE;
                t.weight = 1;

            } else if (tilemap[i][j] == '#'){
                
                t.type = t_WALL;
                t.weight = 1;

            } else if(tilemap[i][j] == '%') {

                t.type = t_WALL;
                t.weight = 1;
                tp.connection_point = true;

            } else if (tilemap[i][j] == '.') {

                t.type = t_FLOOR;
                t.weight = 0;

            } else if (tilemap[i][j] == '=') {

                t.type = t_DOOR_CLOSED;
                t.weight = 0;

            } else if (isdigit(tilemap[i][j]) && tilemap[i][j] != '0') {

                int config = std::stoi(tilemap[i].substr(j, j + 1));

                if (config == door_config) {
                    t.type = t_DOOR_CLOSED;
                    t.weight = 0;
                } else {
                    t.type = t_WALL;
                    t.weight = 1;
                }
                
            } else if (tilemap[i][j] == 'x') {

                if (tileset) {
                    t.type = t_WALL;
                    t.weight = 1;
                } else {
                    t.type = t_FLOOR;
                    t.weight = 0;
                }

            } else if (tilemap[i][j] == '+') {

                if (tileset) {
                    t.type = t_FLOOR;
                    t.weight = 0;
                } else {
                    t.type = t_WALL;
                    t.weight = 1;
                }

            } else if (tilemap[i][j] == '(') {
                
                if (tileset) {
                    t.type = t_DOOR_CLOSED;
                    t.weight = 0;
                } else {
                    t.type = t_WALL;
                    t.weight = 1;
                }

            } else if (tilemap[i][j] == ')') {

                if (tileset) {
                    t.type = t_WALL;
                    t.weight = 1;
                } else {
                    t.type = t_DOOR_CLOSED;
                    t.weight = 0;
                }

            } else if (tilemap[i][j] == '[') {

                if (tileset) {
                    t.type = t_DOOR_CLOSED;
                    t.weight = 0;
                } else {
                    t.type = t_FLOOR;
                    t.weight = 0;
                }

            } else if (tilemap[i][j] == ']') {

                if (tileset) { 
                    t.type = t_FLOOR;
                    t.weight = 0;
                } else {
                    t.type = t_DOOR_CLOSED;
                    t.weight = 0;
                }

            } else if (tilemap[i][j] == '*') {

                //choose if stairwell is up or down
                bool up = rand() % 2;

                if (up) {
                    t.type = t_STAIRS_UP;
                    t.weight = 0;
                } else {
                    t.type = t_STAIRS_DOWN;
                    t.weight = 0;
                }

                // tp.enemy = enemyTemplates[rand() % enemyTemplates.size()].generate();
                // tp.has_enemy = true;
                groups.push_back( std::make_pair( std::make_pair(i, j), groupTemplates[rand() % groupTemplates.size()] ) );

            } else if( tilemap[i][j] == '!' ||
                       tilemap[i][j] == '@' ) {

                t.type = t_FLOOR;
                t.weight = 0;
                groups.push_back( std::make_pair( std::make_pair(i, j), groupTemplates[rand() % groupTemplates.size()] ) );

            } else if( tilemap[i][j] == '-' ||
                       tilemap[i][j] == '$' ) {
                
                t.type = t_FLOOR;
                t.weight = 0;

            } else {

                printf("UNEXPECTED SYMBOL %s ENCOUNTERED IN ROOM %s AT (%d, %d). PLACING A BLANK TILE AT THIS POSITION\n", tilemap[i].substr(j, j + 1).c_str(), name.c_str(), j, i);
                t.type = t_NONE;
                t.weight = 1;
            }

            tp.tile = t;
            row.push_back(tp);
        }

        tilemapProto.push_back(row);
    }

    bool claimed[w][h];
    for (int i = 0; i < w; i ++) {
        for (int j = 0; j < h; j ++) {

            if (tilemapProto[i][j].tile.type == t_FLOOR) {
                claimed[i][j] = false;
            } else {
                claimed[i][j] = true;
            }
        }
    }


    for (auto group : groups) {

        std::pair<int, int> loc = group.first;
        GroupTemplate gtp = group.second;
        std::vector<Enemy> enemies = gtp.generate();

        TileProto top_left;
        top_left.x = constrain(loc.first - ENEMY_SPAWN_RANGE / 2, 0, w);
        top_left.y = constrain(loc.second - ENEMY_SPAWN_RANGE / 2, 0, h);

        TileProto bottom_right;
        bottom_right.x = constrain(loc.first + ENEMY_SPAWN_RANGE / 2, 0, w);
        bottom_right.y = constrain(loc.second + ENEMY_SPAWN_RANGE / 2, 0, h);

        for (Enemy enemy : enemies) {
            bool success = false;
            int tries = 0;
            int x;
            int y;

            while(!(success || tries >= ENEMY_SPAWN_ATTEMPTS)) {

                x = rand() % (bottom_right.x - top_left.x) + top_left.x;
                y = rand() % (bottom_right.y - top_left.y) + top_left.y;

                if (!claimed[x][y]) {
                    success = true;
                }

                tries ++;
            }

            if (success) {
                tilemapProto[x][y].has_enemy = true;
                tilemapProto[x][y].enemy = enemy;
            }
        }
    }

    return tilemapProto;
}

Enemy EnemyTemplate::generate() {

    Enemy e = Enemy(etype);

    if (health.first == health.second) {
        e.health = health.first;
    } else {
        e.health = rand() % (health.second - health.first) + health.first;
    }

    if (armor_class.first == armor_class.second) {
        e.armor_class = armor_class.first;
    } else {
        e.armor_class = rand() % (armor_class.second - armor_class.first) + armor_class.first;
    }

    return e;
}

std::vector<Enemy> GroupTemplate::generate() {

    std::vector<Enemy> enemies;

    for (MONSTER_DEFINITION m_def : templates) {

        int amount;

        if (m_def.first.first == m_def.first.second) {
            amount = m_def.first.first;
        } else {
            amount = rand() % (m_def.first.second - m_def.first.first + 1) + m_def.first.first;
        }

        for (int i = 0; i < amount; i ++) {
            enemies.push_back(m_def.second.generate());
        }

    }

    return enemies;
}

#ifdef DEBUG_TEMPLATE_ROOM_MAIN
    int main() {

        srand(time(NULL));
        std::vector<RoomTemplate> templates = parseRooms();
        
        RoomTemplate rt = templates[6];
        std::vector<std::vector<TileProto>> tm = rt.generate();

        for (size_t i = 0; i < tm.size(); i ++) {
            for (size_t j = 0; j < tm[0].size(); j ++) {
                switch(tm[i][j].tile.type) {
                    
                    case t_NONE:
                    printf(" ");
                    break;

                    case t_WALL:
                    printf("#");
                    break;

                    case t_FLOOR:
                    printf(".");
                    break;

                    case t_DOOR_CLOSED:
                    case t_DOOR_OPEN:
                    printf("=");
                    break;

                    case t_STAIRS_UP:
                    case t_STAIRS_DOWN:
                    printf("^");
                    break;
                }
            }
            printf("\n");
        }
        return 0;
    }
#endif

#ifdef DEBUG_TEMPLATE_ENEMY_MAIN
    int main() {

        srand(time(NULL));

        std::vector<EnemyTemplate> templates = parseEnemies();

        EnemyTemplate etp = templates[1];

        printf("name:%s\nhealth:%d-%d\nac:%d-%d\n", etp.name.c_str(), etp.health.first, etp.health.second, etp.armor_class.first, etp.armor_class.second);
    }
#endif
