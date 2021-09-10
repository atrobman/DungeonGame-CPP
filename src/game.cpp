#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <sstream>

#include "classes.hpp"
#include "tile.hpp"
#include "pathfinding.hpp"
#include "label.hpp"
#include "templates.hpp"

SDL_Texture* gTextures[NUM_TEXTURES];

static int d_sq(int x1, int y1, int x2, int y2) {
	//return the distance squared between two points
	return pow(x1 - x2, 2) + pow(y1 - y2, 2);
}

static std::vector<TileProto> _plotLineLow(Game *game, int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int yi = 1;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}
	int D = 2 * dy - dx;
	int y = y1;

	std::vector<TileProto> output;
	for (int x = x1; x < x2; x ++) {
		TileProto tp;
		tp.tile = game->getTile(x,y);
		tp.x = x;
		tp.y = y;
		output.push_back(tp);
		if (D > 0) {
			y = y + yi;
			D = D - 2 * dx;
		}
		D = D + 2 * dy;
	}

	return output;
}

static std::vector<TileProto> _plotLineHigh(Game *game, int x1, int y1, int x2, int y2) {
	int dx = x2 - x1;
	int dy = y2 - y1;
	int xi = 1;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}
	int D = 2 * dx - dy;
	int x = x1;

	std::vector<TileProto> output;
	for (int y = y1; y < y2; y ++) {
		TileProto tp;
		tp.tile = game->getTile(x,y);
		tp.x = x;
		tp.y = y;
		output.push_back(tp);
		if (D > 0) {
			x = x + xi;
			D = D - 2 * dy;
		}
		D = D + 2 * dx;
	}

	return output;
}

static std::vector<TileProto> getTilesInLine(Game *game, int x1, int y1, int x2, int y2) {
	
	if (abs(y2 - y1) < abs(x2 - x1)) {
		if (x1 > x2) {
			return _plotLineLow(game, x2, y2, x1, y1);
		} else {
			return _plotLineLow(game, x1, y1, x2, y2);
		}
	} else {
		if (y1 > y2) {
			return _plotLineHigh(game, x2, y2, x1, y1);
		} else {
			return _plotLineHigh(game, x1, y1, x2, y2);
		}
	}
}

Game::Game() {
    gWindow = NULL;
    gScreenSurface = NULL;
	player = Player();
	level = 0;
	font = NULL;
	roomTemplates = parseRooms();
	enemyTemplates = parseEnemies();
	groupTemplates = parseGroups(enemyTemplates);
	round = 0;
	playerTurn = true;

	for (int i = 0; i < NUM_TEXTURES; i ++) {
		gTextures[i] = NULL;
	}
	
	for (int i = 0; i < DUNGEON_WIDTH; i ++) {
		std::vector<Tile> row;
		for (int j = 0; j < DUNGEON_HEIGHT; j ++) {
			row.push_back(Tile());
		}
		tilemap.push_back(row);
	}
}

Game::~Game() {
    return;
}

bool Game::init() {

	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Crete renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if (gRenderer == NULL) {
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			} else {

				SDL_SetRenderDrawBlendMode( gRenderer, SDL_BLENDMODE_BLEND );
				SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!( IMG_Init( imgFlags ) & imgFlags ) ) {
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
				else {

					if ( TTF_Init() == -1) {
						printf( "SDL_ttf could not be initalized! SDL_ttf Error: %s\n", TTF_GetError() );
						success = false;
					} else {
						font = TTF_OpenFont("fonts/FreeSans.ttf", 20);
						test = Label(font);

						//Get window surface
						gScreenSurface = SDL_GetWindowSurface( gWindow );
					}
				}
			}
		}
	}

	return success;
}

void Game::init_tilemap() {

	for (int i = 0; i < DUNGEON_WIDTH; i ++) {
		for (int j = 0; j < DUNGEON_HEIGHT; j ++) {

			tilemap[i][j].texture = gTextures[x_WALL];
			tilemap[i][j].weight = 1;
			tilemap[i][j].type = t_WALL;
			tilemap[i][j].unseen = true;
		}
	}

}

SDL_Texture* Game::loadTexture( std::string path ) {

	#ifdef DEBUG
		printf( "Loading image: %s\n", path.c_str() );
	#endif

	SDL_Texture* newTexture = NULL;
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if (loadedSurface == NULL) {
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		#ifdef DEBUG
			printf( "Rendering image to texture: %s\n", path.c_str() );
		#endif

        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL ) {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}

bool Game::loadMedia() {
	//Loading success flag
	bool success = true;
	
	gTextures[ x_MISSING ] = loadTexture( "images/missing.png" );
	gTextures[ x_FLOOR ] = loadTexture( "images/floor.png" );
	gTextures[ x_WALL ] = loadTexture( "images/wall.png" );
	gTextures[ x_DOOR ] = loadTexture( "images/door.png" );
	gTextures[ x_PLAYER ] = loadTexture( "images/player.png" );
	gTextures[ x_STAIRS_UP ] = loadTexture( "images/stairs_up.png" );
	gTextures[ x_STAIRS_DOWN ] = loadTexture( "images/stairs_down.png" );
	gTextures[ x_HIDDEN ] = loadTexture( "images/hidden.png" );
	gTextures[ x_DOOR_OPEN] = loadTexture( "images/door_open.png" );
	gTextures[ x_KOBOLD ] = loadTexture( "images/kobold.png" );
	gTextures[ x_GOBLIN ] = loadTexture( "images/goblin.png" );
	gTextures[ x_HOBGOBLIN ] = loadTexture( "images/hobgoblin.png" );
	gTextures[ x_ORC_BASIC ] = loadTexture( "images/orc.png" );
	gTextures[ x_ORC_WARRIOR ] = loadTexture( "images/orc_warrior.png" );
	gTextures[ x_ORC_WARLORD ] = loadTexture( "images/orc_warlord.png" );
	
	for (int i = 0; i < NUM_TEXTURES; i ++) {
		success &= !(gTextures[i] == NULL);
	}

	player.texture = gTextures[ x_PLAYER ];

	return success;
}

void Game::draw() {

	int x_center = SCREEN_WIDTH/2 - TILE_SIZE/2;
	int y_center = SCREEN_HEIGHT/2 - TILE_SIZE/2;

	int x_offset = x_center - player.x * TILE_SIZE;
	int y_offset = y_center - player.y * TILE_SIZE;

	SDL_RenderClear( gRenderer );

	SDL_Rect r;

	//draw tilemap
	for (int i = 0; i < DUNGEON_WIDTH; i ++) {
		for (int j = 0; j < DUNGEON_HEIGHT; j ++) {
			r = {i * TILE_SIZE + x_offset, j * TILE_SIZE + y_offset, TILE_SIZE, TILE_SIZE};
			int dist = d_sq(i, j, player.x, player.y);

			if ( dist <= RENDER_DISTANCE * RENDER_DISTANCE) {

				if (!tilemap[i][j].unseen) {
					if (SDL_RenderCopy( gRenderer, tilemap[i][j].texture, NULL, &r ) == -1) {
						printf("Failed to render tile (%d, %d)! SDL Error: %s\n", i, j, SDL_GetError());
					}
				}

				bool show_tile = (dist <= player.light_radius * player.light_radius);

				if (show_tile) {
					auto tiles = getTilesInLine(this, i, j, player.x, player.y);
					for (TileProto tile : tiles) {
						if ((tile.tile.type == t_WALL || tile.tile.type == t_DOOR_CLOSED) && !(tile.x == i && tile.y == j)) {
							show_tile = false;
							break;
						}
					}
				}

				if ( !show_tile ) {
					SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0x7F );
					SDL_RenderFillRect( gRenderer, &r);
					SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0xFF );
				} else {
					tilemap[i][j].unseen = false;
				}
			}
		}
	}

	//draw enemies
	for (Enemy e : enemies) {
		if (e.etype != e_PLAYER) {
			r = {e.x * TILE_SIZE + x_offset, e.y * TILE_SIZE + y_offset, TILE_SIZE, TILE_SIZE};
			SDL_RenderCopy(gRenderer, e.texture, NULL, &r);
		}
	}
	
	//draw player
	r = {x_center, y_center, TILE_SIZE, TILE_SIZE};
	if (SDL_RenderCopy( gRenderer, player.texture, NULL, &r) == -1) {
		printf("Failed to render player! SDL Error: %s\n", SDL_GetError());
	}

	//draw text
	std::ostringstream text;
	text << "Player position: (" << player.x << ", " << player.y << ")   Round #" << round;

	test.setText(gRenderer, text.str());
	test.draw(gRenderer, 0, 0);
	
	SDL_RenderPresent( gRenderer );

}

void Game::close() {
	//Deallocate surface
	for (int i = 0; i < NUM_TEXTURES; i ++) {
		SDL_DestroyTexture( gTextures[i] );
		gTextures[i] = NULL;
	}

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void Game::move_player_to(int x, int y) {

	if (playerTurn) {
		if ((x == player.x + 1 || x == player.x - 1 || x == player.x) && (y == player.y + 1 || y == player.y - 1 || y == player.y) && (x != player.x || y != player.y)) {

			if (tilemap[x][y].type == t_DOOR_CLOSED) {
				tilemap[x][y].type = t_DOOR_OPEN;
				tilemap[x][y].texture = gTextures[ x_DOOR_OPEN ];
				turn();
			} else if (tilemap[x][y].weight == 0) {
				player.x = x;
				player.y = y;
				turn();
			}

		} else if (((x != player.x) || (y != player.y)) && !tilemap[x][y].unseen) {

			Pathfinder* pathfinder = new Pathfinder(DUNGEON_WIDTH, DUNGEON_HEIGHT);

			for (int i = 0; i < DUNGEON_WIDTH; i ++) {
				for (int j = 0; j < DUNGEON_HEIGHT; j ++) {
					if (tilemap[i][j].weight == 1) {
						pathfinder->makeWall(i, j);
					}
				}
			}

			std::vector<Pathfinder::Node> path = pathfinder->aStar(
				Pathfinder::Node(player.x, player.y), Pathfinder::Node(x, y)
			);

			for (size_t i = 0; i < path.size(); i ++) {

				Tile tile = tilemap[path[i].x][path[i].y];

				//Open closed doors on the path
				if (tile.type == t_DOOR_CLOSED) {
					tilemap[path[i].x][path[i].y].type = t_DOOR_OPEN;
					tilemap[path[i].x][path[i].y].texture = gTextures[ x_DOOR_OPEN ];
				}

				//Move the player
				if (path[i].x > player.x) {
					player.x ++;
					turn();
				} else if (path[i].x < player.x) {
					player.x --;
					turn();
				}

				if (path[i].y > player.y) {
					player.y ++;
					turn();
				} else if (path[i].y < player.y) {
					player.y --;
					turn();
				}

				draw();
			}
		}
	}
}

Tile Game::getTile(int x, int y) {

	if (x >= 0 && x < DUNGEON_WIDTH && y >= 0 && y < DUNGEON_HEIGHT) {
		return tilemap[x][y];
	}

	return Tile();
}

void Game::generate_dungeon(bool up) {
	
	bool claimed[DUNGEON_WIDTH][DUNGEON_HEIGHT];
	for (int i = 0; i < DUNGEON_WIDTH; i ++) {
		for (int j = 0; j < DUNGEON_HEIGHT; j ++) {
			claimed[i][j] = false;
		}
	}

	//pick a random number between NUMBER_ROOMS_MIN and NUMBER_ROOMS_MAX for the number of rooms to generate
	int num_rooms = rand() % (NUMBER_ROOMS_MAX - NUMBER_ROOMS_MIN) + NUMBER_ROOMS_MIN;

	enemies = std::vector<Enemy>();

	std::vector<std::vector<TileProto>> connection_points;

	std::vector<std::pair<TileProto, TileProto>> room_gen;

	int tries = 0;
	int rooms_failed = 0;

	//generate the random rooms so that they don't overlap
	//try several times before giving up and moving on to the next room
	for (int i = 0; i < num_rooms; i ++) {
		std::vector<TileProto> row;

		//pick a random room template and generate it's tilemap
		RoomTemplate t = roomTemplates[rand() % roomTemplates.size()];
		std::vector<std::vector<TileProto>> template_tilemap = t.generate(groupTemplates);

		//pick a random tile for the top left hand corner of the room
		//(make sure the whole room will fit in bounds from this tile)
		int start_x = rand() % (DUNGEON_WIDTH - t.w);
		int start_y = rand() % (DUNGEON_HEIGHT - t.h);
		
		bool valid_spot = true;

		for (int j = 0; j < t.w; j ++) {
			for (int k = 0; k < t.h; k ++) {
				valid_spot &= !claimed[start_x + j][start_y + k] || template_tilemap[j][k].tile.type == t_NONE;
			}
		}
		
		if (valid_spot) {			
			for (int j = 0; j < t.w; j ++) {
				for (int k = 0; k < t.h; k ++) {
					int x = j + start_x;
					int y = k + start_y;

					if (template_tilemap[j][k].has_enemy) {
						Enemy e = template_tilemap[j][k].enemy;
						e.x = x;
						e.y = y;
						enemies.push_back(e);
					}

					claimed[x][y] = true;

					if (template_tilemap[j][k].tile.type != t_NONE) {
						tilemap[x][y] = template_tilemap[j][k].tile;
						
						if (template_tilemap[j][k].connection_point) {
							TileProto tp;
							tp.connection_point = true;
							tp.tile = template_tilemap[j][k].tile;
							tp.x = x;
							tp.y = y;

							row.push_back(tp);
						}
						
						switch(tilemap[x][y].type) {

						case t_NONE:
							tilemap[x][y].texture = gTextures[x_PLAYER];
							break;

						case t_WALL:
							tilemap[x][y].texture = gTextures[x_WALL];
							break;

						case t_FLOOR:
							tilemap[x][y].texture = gTextures[x_FLOOR];
							break;

						case t_DOOR_CLOSED:
							tilemap[x][y].texture = gTextures[x_DOOR];
							break;

						case t_STAIRS_DOWN:
							tilemap[x][y].texture = gTextures[x_STAIRS_DOWN];
							break;

						case t_STAIRS_UP:
							tilemap[x][y].texture = gTextures[x_STAIRS_UP];
							break;
						
						case t_DOOR_OPEN:
							tilemap[x][y].texture = gTextures[x_DOOR_OPEN];
						}
					}
				}
			}

			TileProto top_left;
			TileProto bottom_right;

			top_left.x = start_x;
			top_left.y = start_y;
			bottom_right.x = start_x + t.w;
			bottom_right.y = start_y + t.h;

			connection_points.push_back(row);
			room_gen.push_back(std::make_pair(top_left, bottom_right));
			tries = 0;
		} else {

			tries ++;

			if (tries > MAX_ROOM_GEN_TRIES) {
				tries = 0;
				rooms_failed ++;
			} else { 
				i --;
			}
		}
	}

	//try and link the rooms with corridors in a great circle
	//(i.e. try to link room 1 connects to rooms 2-n, room 2 connects to rooms 3-n, etc.)
	//if the corridor goes for longer then MAX_CORRIDOR_LENGTH, abort creating the corridor
	//if the corridor runs into a floor tile (another room or another corridor), stop prematurely
	for (int i = 0; i < num_rooms - 1 - rooms_failed; i ++) {

		for (int j = i + 1; j < num_rooms - rooms_failed; j ++) {
			
			bool path_valid = false;

			std::vector<std::pair<int, int>> path;

			//pick a random connection point from room j
			TileProto dist_tile = connection_points[j][rand() % connection_points[j].size()];

			//find the closest connection point to the dist_tile in room i
			TileProto closest_start = connection_points[i][0];
			for (TileProto tile : connection_points[i]) {

				if ( d_sq(closest_start.x, closest_start.y, dist_tile.x, dist_tile.y) > d_sq(tile.x, tile.y, dist_tile.x, dist_tile.y) ) {
					closest_start = tile;
				}
			}

			TileProto closest_goal = connection_points[j][0];
			for (TileProto tile : connection_points[j]) {

				if ( d_sq(closest_goal.x, closest_goal.y, closest_start.x, closest_start.y) > d_sq(tile.x, tile.y, closest_start.x, closest_start.y) ) {
					closest_goal = tile;
				}
			}
			//pick a random connection point from room i to start from
			TileProto start = closest_start;

			//pick a random connection point from room j to move to
			TileProto goal = closest_goal;
	
			//perform a walk that moves from the start to the goal
			path.push_back(std::make_pair(start.x, start.y));

			while(!(path.size() >= MAX_CORRIDOR_LENGTH) && !path_valid) {

				bool try_up_first = rand() % 2;
				std::pair<int, int> last = path[path.size() - 1];
				if (last.first == goal.x && last.second == goal.y) {
					path_valid = true;
				} else if (tilemap[last.first][last.second].type == t_FLOOR) {
					break;
				}
				else {

					if (try_up_first) {
						
						if (last.second > goal.y) {
							path.push_back(std::make_pair(last.first, last.second - 1));
						} else if (last.second < goal.y) {
							path.push_back(std::make_pair(last.first, last.second + 1));
						} else if (last.first > goal.x) {
							path.push_back(std::make_pair(last.first - 1, last.second));
						} else if (last.first < goal.x) {
							path.push_back(std::make_pair(last.first + 1, last.second));
						}

					} else {

						if (last.first > goal.x) {
							path.push_back(std::make_pair(last.first - 1, last.second));
						} else if (last.first < goal.x) {
							path.push_back(std::make_pair(last.first + 1, last.second));
						} else if (last.second > goal.y) {
							path.push_back(std::make_pair(last.first, last.second - 1));
						} else if (last.second < goal.y) {
							path.push_back(std::make_pair(last.first, last.second + 1));
						}

					}
				
				}
			}

			if (path_valid) {
				for (auto coord : path) {
					
					if (tilemap[coord.first][coord.second].type == t_FLOOR) {
						break;
					}

					tilemap[coord.first][coord.second].texture = gTextures[x_FLOOR];
					tilemap[coord.first][coord.second].type = t_FLOOR;
					tilemap[coord.first][coord.second].weight = 0;

				}
			}
		}
	}

	//if up is true, try to place the player on a down stairwell
	//otherwise, try to place the player on an up stairwell
	//if no stairwells were created in the correct orientation,
	//choose a random floor tile and turn it into a correct stairwell
	std::vector<std::pair<int, int>> spawn_point_options;
	for (int i = 0; i < DUNGEON_WIDTH; i ++) {
		for (int j = 0; j < DUNGEON_HEIGHT; j ++) {
			if ((up && tilemap[i][j].type == t_STAIRS_UP) || (!up && tilemap[i][j].type == t_STAIRS_DOWN)){
				spawn_point_options.push_back(std::make_pair(i, j));
			}
		}
	}

	if (spawn_point_options.size() != 0) {
		auto spawn_loc = spawn_point_options[rand() % spawn_point_options.size()];
		player.x = spawn_loc.first;
		player.y = spawn_loc.second;
	} else {
		bool exit = false;
		while (!exit) {
			int x = rand() % DUNGEON_WIDTH;
			int y = rand() % DUNGEON_HEIGHT;

			if (tilemap[x][y].type == t_FLOOR) {
				player.x = x;
				player.y = y;
				exit = true;

				if (up) {
					tilemap[x][y].type = t_STAIRS_UP;
					tilemap[x][y].texture = gTextures[ x_STAIRS_UP ];
					tilemap[x][y].weight = 0;
				} else {
					tilemap[x][y].type = t_STAIRS_DOWN;
					tilemap[x][y].texture = gTextures[ x_STAIRS_DOWN ];
					tilemap[x][y].weight = 0;
				}
			}
		}
	}
	
	round = 0;
	playerTurn = true;
}

void Game::turn() {

	for (size_t i = 0; i < enemies.size(); i ++) {
		enemies[i].turn(this);
	}

	playerTurn = true;
	round ++;
	draw();
}