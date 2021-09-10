#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #include "game.hpp"
#include "classes.hpp"
#include "tile.hpp"

#define UP true
#define DOWN false

int main( int argc, char* args[] )
{

	srand(time(NULL));

	Game game = Game();

	//Start up SDL and create window
	if( !game.init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !game.loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{

			game.init_tilemap();
			game.generate_dungeon(UP);

			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;
			
			//Misc runtime variables
			Tile tile;

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					} else if ( e.type == SDL_KEYDOWN ) {
						
						switch( e.key.keysym.sym ) {

							case SDLK_f:
							game.init_tilemap();
							game.generate_dungeon(UP);
							break;

							case SDLK_g:
							for (int i = 0; i < DUNGEON_WIDTH; i ++) {
								for (int j = 0; j < DUNGEON_HEIGHT; j ++) {
									game.tilemap[i][j].unseen = false;
								}
							}
							break;

							case SDLK_h:
							game.turn();
							break;
							
							case SDLK_COMMA:
							tile = game.getTile(game.player.x, game.player.y);

							if (tile.type == t_STAIRS_UP) {
								game.level --;
								game.init_tilemap();
								game.generate_dungeon(DOWN);
							}
							break;

							case SDLK_PERIOD:
							tile = game.getTile(game.player.x, game.player.y);

							if (tile.type == t_STAIRS_DOWN) {
								game.level ++;
								game.init_tilemap();
								game.generate_dungeon(UP);
							}
							break;

							case SDLK_UP:
							case SDLK_KP_8:
							game.move_player_to(game.player.x, game.player.y - 1);
							break;

							case SDLK_DOWN:
							case SDLK_KP_2:
							game.move_player_to(game.player.x, game.player.y + 1);
							break;

							case SDLK_LEFT:
							case SDLK_KP_4:
							game.move_player_to(game.player.x - 1, game.player.y);
							break;

							case SDLK_RIGHT:
							case SDLK_KP_6:
							game.move_player_to(game.player.x + 1, game.player.y);
							break;
							
							case SDLK_KP_1:
							game.move_player_to(game.player.x - 1, game.player.y + 1);
							break;

							case SDLK_KP_7:
							game.move_player_to(game.player.x - 1, game.player.y - 1);
							break;

							case SDLK_KP_9:
							game.move_player_to(game.player.x + 1, game.player.y - 1);
							break;

							case SDLK_KP_3:
							game.move_player_to(game.player.x + 1, game.player.y + 1);
							break;
							
							case SDLK_q:
							quit = true;
							break;
							
							default:
							break;

						}
					
					} else if ( e.type == SDL_MOUSEBUTTONDOWN ) {

						int x, y;
						SDL_GetMouseState( &x, &y );

						game.move_player_to(
											(x - SCREEN_WIDTH/2 + TILE_SIZE/2 + game.player.x * TILE_SIZE) / TILE_SIZE,
											(y - SCREEN_HEIGHT/2 + TILE_SIZE/2 + game.player.y * TILE_SIZE) / TILE_SIZE
											);
						
					}
				}

				game.draw();
			}
		}
	}

	//Free resources and close SDL
	game.close();

	return 0;
}