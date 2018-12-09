//Territory Main File

//Using SDL and standard IO
#include "view.h"
#include <stdio.h>

/*
Add Player Sprite
Add Game Mode Switch Button
Add Smarter Local Tiling, No edge, more efficiency
Add Rotate Map Option?
Fix Coordinate System
Improve Local Terrain Generator
	Fix Water Level
Add Randomized Floor Sprites for more interesting Floor
Add Basic Grass and Shrub Foliage
Attempt to add Trees?

- Save Global Depth Map to File
*/

//Screen dimension constants
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

//Function Definitions
void drawWorldMap(World terrain, SDL_Renderer* gRenderer, Player player);
void drawWorldOverlay(World terrain, SDL_Renderer* gRenderer, int a);

bool loadMedia();

int main( int argc, char* args[] ) {
	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;
	SDL_Renderer* gRenderer = NULL;

	TTF_Init();

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow( "Territory", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Prepare the Renderer
		  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);

			//Tiling Logic
			View view;
			if(!view.loadTilemap(*&gRenderer)){
				std::cout<<"Couldn't load file."<<std::endl;
			}

			//World Generation
			World territory;
			Player player;

			territory.generate();
			//Clear the Screen
			SDL_SetRenderDrawBlendMode(gRenderer,SDL_BLENDMODE_BLEND);

			//Game Loop
			bool quit = false;
			SDL_Event e;

			while(!quit){
				//Check for Quit
				while( SDL_PollEvent( &e ) != 0 ) {
					//User requests quit
					if( e.type == SDL_QUIT ) { quit = true; }
					else if( e.type == SDL_KEYDOWN ) {
						if (e.key.keysym.sym == SDLK_SPACE){
							view.switchView();
						}
						else if (e.key.keysym.sym == SDLK_r){
							view.rotateView();
						}
						if(view.viewMode == 1){
							territory.changePos(e);
						}
						else if(view.viewMode == 2){
							player.changePos(e);
						}
					}
				}

				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
				SDL_RenderClear(gRenderer);
				if(view.viewMode == 0){
					territory.day+=1;
					territory.climate.calcWind(territory.day, territory.seed, territory.terrain);
					territory.climate.calcTempMap(territory.terrain);
					territory.climate.calcHumidityMap(territory.terrain);
					territory.climate.calcDownfallMap();

					//I don't know why this works
					drawWorldMap(territory, *&gRenderer, player);
					//drawWorldOverlay(territory, *&gRenderer,4);

					//Wait for day development
					//view.calcFPS();
					SDL_Delay(100);
				}

				else if(view.viewMode == 1){
					view.renderMap(territory, gRenderer, territory.xview, territory.yview);
					//view.calcFPS();
				}

				else if(view.viewMode == 2){
					view.renderLocal(territory, gRenderer, player);
					//view.calcFPS();
					SDL_Delay(10);
				}
				//Draw Everything
				SDL_RenderPresent(gRenderer);
			}
		}
	}
	//Destroy window
	SDL_DestroyWindow( gWindow );
	SDL_DestroyRenderer (gRenderer);

	//Quit SDL subsystems
  TTF_Quit();
	SDL_Quit();

	return 0;
}

void drawWorldMap(World territory, SDL_Renderer* gRenderer, Player player){
	//Draw the Map with Overlays
	for (int i=0; i<100; i++){
		for (int j=0; j<100; j++){
			int a = territory.terrain.biomeMap[i][j];
			SDL_Rect rect;
			rect.x=i*10;
			rect.y=j*10;
			rect.w=10;
			rect.h=10;
			switch(a){
				//Water
				case 0: SDL_SetRenderDrawColor(gRenderer, 0x2d, 0x56, 0x85, 255);
				break;
				//Sandy Beach
				case 1: SDL_SetRenderDrawColor(gRenderer, 0xea, 0xdf, 0x9e, 255);
				break;
				//Gravel Beach
				case 2: SDL_SetRenderDrawColor(gRenderer, 0xcc, 0xcc, 0xcc, 255);
				break;
				//Stoney Beach Cliff
				case 3: SDL_SetRenderDrawColor(gRenderer, 0xa7, 0xa5, 0x9b, 255);
				break;
				//Wet Plains (Grassland)
				case 4: SDL_SetRenderDrawColor(gRenderer, 0x9e, 0xc1, 0x6d, 255);
				break;
				//Dry Plains (Shrubland)
				case 5: SDL_SetRenderDrawColor(gRenderer, 0xbc, 0xc1, 0x6d, 255);
				break;
				//Rocky Hills
				case 6: SDL_SetRenderDrawColor(gRenderer, 0xaa, 0xaa, 0xaa, 255);
				break;
				//Temperate Forest
				case 7: SDL_SetRenderDrawColor(gRenderer, 0x3d, 0xab, 0x50, 255);
				break;
				//Boreal Forest
				case 8: SDL_SetRenderDrawColor(gRenderer, 0x30, 0x7a, 0x3c, 255);
				break;
				//Mountain Tundra
				case 9: SDL_SetRenderDrawColor(gRenderer, 0x77, 0x77, 0x77, 255);
				break;
				//Mountain Peak
				case 10: SDL_SetRenderDrawColor(gRenderer, 0xee, 0xee, 0xee, 255);
			}
			SDL_RenderFillRect(gRenderer, &rect);
		}
	}
	SDL_SetRenderDrawColor(gRenderer, 0xee, 0x11, 0x11, 255);
	SDL_Rect rect;
	rect.x=player.xGlobal*10;
	rect.y=player.yGlobal*10;
	rect.w=10;
	rect.h=10;
	SDL_RenderFillRect(gRenderer, &rect);
}

void drawWorldOverlay(World territory, SDL_Renderer* gRenderer, int a){
	//Draw the Climate Map
	for(int i = 0; i<100; i++){
		for(int j = 0; j<100; j++){

			//std::cout<<territory.climate.AvgRainMap[i][j]<<std::endl;
			//Drawing Rectangle
			SDL_Rect rect;
			rect.x=i*10;
			rect.y=j*10;
			rect.w=10;
			rect.h=10;
			//Render the Overlays
			switch(a){
				//Wind Map
				case 0: SDL_SetRenderDrawColor(gRenderer, territory.climate.WindMap[i][j]*25, territory.climate.WindMap[i][j]*25, territory.climate.WindMap[i][j]*25, 100);
					break;
				//Cloud Map
				case 1: SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 100*territory.climate.CloudMap[i][j]);
					break;
				//Rain Map
				case 2: SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255*territory.climate.RainMap[i][j]);
					break;
				//Temperature Map
				case 3: SDL_SetRenderDrawColor(gRenderer, territory.climate.TempMap[i][j]*255, 150, 150, 100);
					break;
				//Humidity Map
				case 4: SDL_SetRenderDrawColor(gRenderer, 50, 50, territory.climate.HumidityMap[i][j]*255, 220);
					break;
				//Average Wind Map
				case 5: SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, ((5-territory.climate.AvgWindMap[i][j])+2)*60);
					break;
				//Average Cloud Map
				case 6: SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255*territory.climate.AvgCloudMap[i][j]);
					break;
				//Average Rain Map
				case 7: SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255*10*territory.climate.AvgRainMap[i][j]);
					break;
				//Average Temperature Map
				case 8: SDL_SetRenderDrawColor(gRenderer, territory.climate.AvgTempMap[i][j]*255, 150, 150, 100);
					break;
				//Average Humidity Map
				case 9: SDL_SetRenderDrawColor(gRenderer, 50, 50, territory.climate.AvgHumidityMap[i][j]*255, 220);
					break;

			}
			SDL_RenderFillRect(gRenderer, &rect);
		}
	}
}
