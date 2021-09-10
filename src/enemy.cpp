#include <SDL.h>

#include "enemy.hpp"
#include "textures.hpp"
#include "pathfinding.hpp"
#include "tile.hpp"

static int d_sq(int x1, int y1, int x2, int y2) {
	//return the distance squared between two points
	return pow(x1 - x2, 2) + pow(y1 - y2, 2);
}

Enemy::Enemy() {

    x = -1;
    y = -1;
    health = 0;
    armor_class = 0;
    texture = gTextures[ x_MISSING ];
    etype = e_NONE;
    aiType = ai_NONE;
}

Enemy::Enemy(ENEMY_TYPE type) {

    x = -1;
    y = -1;
    health = 0;
    armor_class = 0;
    etype = type;

    switch(type) {

        case e_NONE:
        case e_PLAYER:
        texture = gTextures[ x_MISSING ];
        aiType = ai_NONE;
        break;

        case e_KOBOLD:
        texture = gTextures[ x_KOBOLD ];
        aiType = ai_MELEE;
        break;

        case e_GOBLIN:
        texture = gTextures[ x_GOBLIN ];
        aiType = ai_RANDOM;
        break;

        case e_HOBGOBLIN:
        texture = gTextures[ x_HOBGOBLIN ];
        aiType = ai_MELEE;
        break;
        
        case e_ORC_BASIC:
        texture = gTextures[ x_ORC_BASIC ];
        aiType = ai_MELEE;
        break;

        case e_ORC_WARRIOR:
        texture = gTextures[ x_ORC_WARRIOR ];
        aiType = ai_MELEE;
        break;

        case e_ORC_WARLORD:
        texture = gTextures[ x_ORC_WARLORD ];
        aiType = ai_MELEE;
        break;

        default:
        texture = gTextures[ x_MISSING ];
        aiType = ai_NONE;
        break;

    }
}

Enemy::~Enemy() {

    x = -1;
    y = -1;
    health = 0;
    armor_class = 0;
    texture = gTextures[ x_MISSING ];

}

void Enemy::turn(Game* game) {

    switch(aiType) {

        case ai_NONE:
        {
            //This AI does nothing
            break;
        }

        case ai_RANDOM:
        {
            //This AI will move entirely randomly
            int rand_x = rand() % 3 - 1;
            int rand_y = rand() % 3 - 1;

            if (x + rand_x >= 0 && (size_t)(x + rand_x) < game->tilemap.size() && y + rand_y >= 0 && (size_t)(y + rand_y) < game->tilemap[0].size() && game->tilemap[x+rand_x][y+rand_y].weight == 0) {
                x += rand_x;
                y += rand_y;
            }        
            break;
        }

        case ai_MELEE:
        {
            //This AI will rush towards the player if they are within range
            if (d_sq(x, y, game->player.x, game->player.y) <= 15 * 15) {
                Pathfinder* pathfinder = new Pathfinder(game->tilemap.size(), game->tilemap[0].size());

                for (int i = 0; i < DUNGEON_WIDTH; i ++) {
                    for (int j = 0; j < DUNGEON_HEIGHT; j ++) {
                        if (game->tilemap[i][j].weight == 1) {
                            pathfinder->makeWall(i, j);
                        }
                    }
                }

                std::vector<Pathfinder::Node> path = pathfinder->aStar(
                    Pathfinder::Node(x, y), Pathfinder::Node(game->player.x, game->player.y)
                );

                if (path.size() > 1) {
                    if (path[1].x > x) {
                        x ++;
                    } else if (path[1].x < x) {
                        x --;
                    }

                    if (path[1].y > y) {
                        y ++;
                    } else if (path[1].y < y) {
                        y --;
                    }
                }
            } else {
                aiType = ai_RANDOM;
                turn(game);
                aiType = ai_MELEE;
            }
            break;
        }
    }    
}