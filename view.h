//Tilemapping Class
#include <iostream>
#include <stdlib.h>
#include "worldgen.h"
#include "game.h"
#include <time.h>

//Texture wrapper class
class View {
 public:
   //For FPS Counter
   int ticks= 0;
   int FPS = 0;

   //Game Viewmode
   int viewMode = 0;
   int viewRotation = 0;
   const int renderDistance = 15;

   //Drawing Functions
	 bool loadTilemap(SDL_Renderer* gRenderer);
   void writeText(SDL_Renderer* gRenderer);
   void calcFPS();

   //Overlay Rendering
   void renderMap(World territory, SDL_Renderer* gRenderer, int xview, int yview);
   void renderLocal(World territory, SDL_Renderer* gRenderer, Player player);
   void renderVegetation(World territory, SDL_Renderer* gRenderer, Player player, int i, int j, int tileScale);
   void renderPlayer(World territory, SDL_Renderer* gRenderer, Player player);

   //View altering Functions
   void switchView();
   void rotateView();

 private:
   SDL_Texture* mTexture = NULL;
   SDL_Texture* treeTexture = NULL;
   TTF_Font * gFont = NULL;
};

void View::switchView(){
  viewMode = (viewMode+1)%3;
}

void View::rotateView(){
  viewRotation = (viewRotation+1)%2;
}

void View::calcFPS(){
  FPS = 1000/(SDL_GetTicks()-ticks);
  ticks = SDL_GetTicks();
  std::cout<<FPS<<std::endl;
}

void View::writeText(SDL_Renderer* gRenderer){
  //Load the Font in the desired Size
  gFont = TTF_OpenFont("font.ttf", 50);
  //Set textcolor
  SDL_Color color = { 255, 255, 255 };
  //Render the Text
  SDL_Surface * surface = TTF_RenderText_Solid(gFont, "100", color);
  SDL_Texture * texture = SDL_CreateTextureFromSurface(gRenderer, surface);
  SDL_Rect rect;
    //Replace this with logic based on territory.terrain.surfaceMap[i][j];
    rect.x=0;
    rect.y=0;
    SDL_QueryTexture(texture,NULL,NULL,&rect.w,&rect.h);
  SDL_RenderCopy(gRenderer, texture, NULL, &rect);
}

bool View::loadTilemap(SDL_Renderer* gRenderer){
  mTexture = IMG_LoadTexture( gRenderer, "tiles.png" );
  treeTexture = IMG_LoadTexture( gRenderer, "trunk2.png" );
  if(mTexture != NULL){
    return 1;
  }
  else return 0;
}

void View::renderMap(World territory, SDL_Renderer* gRenderer, int xview, int yview) {
	//Set rendering space and render to screen
  //Isometric Tiling Logic Based on Height and Surface Map
  int tileScale = 5;
  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      //For the Depth
      for(int k =0; k<40; k++){
        //Take Sourcequad from Territory Surface Tile
        SDL_Rect sourceQuad;
          //Replace this with logic based on territory.terrain.surfaceMap[i][j];
          sourceQuad.x=0;
          sourceQuad.y=territory.terrain.biomeMap[i][j]*11;
          sourceQuad.w=11;
          sourceQuad.h=11;
        //Take Renderquad from current i and j numbers
        //Height is in Intervals of 100
        if((int)territory.terrain.depthMap[i][j]/100>=k){
          SDL_Rect renderQuad;
            renderQuad.x=((territory.terrain.worldWidth-tileScale*10)/2)+j*tileScale*5-i*tileScale*5-territory.xview;
            renderQuad.y=j*3*tileScale+i*3*tileScale-territory.yview-k*5*tileScale;
            renderQuad.w=tileScale*11;
            renderQuad.h=tileScale*11;
          //Render
        if(renderQuad.x > -10 && renderQuad.x < 1000 && renderQuad.y < 1000 && renderQuad.y > -10){
          SDL_RenderCopy( gRenderer, mTexture, &sourceQuad, &renderQuad);
        }
        }
      }
    }
  }
  SDL_Rect rect;
  rect.x=1000;
  rect.y=0;
  rect.w=400;
  rect.h=1000;
  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 100);
  SDL_RenderFillRect(gRenderer, &rect);
}

void View::renderPlayer(World territory, SDL_Renderer* gRenderer, Player player){
  treeTexture = IMG_LoadTexture( gRenderer, "trunk.png" );
  SDL_Rect sourceQuad;
  int tileScale = 5;
    sourceQuad.x=0;
    sourceQuad.y=0;
    sourceQuad.w=11;
    sourceQuad.h=22;

    SDL_Rect renderQuad;
    int i = 24;
    int j = 24;
    renderQuad.w=tileScale*11;
    renderQuad.h=tileScale*22;
    renderQuad.x=territory.terrain.worldWidth/2+tileScale*5*(-1);
    renderQuad.y=territory.terrain.worldHeight/2-tileScale*5-tileScale*2*11-((int)territory.terrain.localMap[i][j]-(int)territory.terrain.localMap[24][24])*5*tileScale;
    SDL_RenderCopy( gRenderer, treeTexture, &sourceQuad, &renderQuad);
}

void View::renderVegetation(World territory, SDL_Renderer* gRenderer, Player player, int i, int j, int tileScale){
  //If there is a tree present at given location
  if(territory.vegetation.getTree(territory, player, i, j)){
    SDL_Rect sourceQuad;
      sourceQuad.x=0;
      sourceQuad.y=0;
      sourceQuad.w=11;
      sourceQuad.h=22;

      SDL_Rect renderQuad;
      renderQuad.w=tileScale*11;
      renderQuad.h=tileScale*22;
      renderQuad.x=territory.terrain.worldWidth/2+tileScale*5*(-1+j-i);
      renderQuad.y=territory.terrain.worldHeight/2-tileScale*5-tileScale*17+3*tileScale*((j-25)+(i-25))-((int)territory.terrain.localMap[i][j]-(int)territory.terrain.localMap[24][24])*5*tileScale;
      SDL_RenderCopy( gRenderer, treeTexture, &sourceQuad, &renderQuad);
  }
}

void View::renderLocal(World territory, SDL_Renderer* gRenderer, Player player){
  //Set rendering space and render to screen
  //Generate the Local Area
  //Isometric Tiling Logic Based on Height and Surface Map
  int tileScale = 6;
  territory.terrain.genLocal(territory.seed, player);
        for(int i=0; i<50; i++){
          for(int j=0; j<50; j++){
              //Take Sourcequad from Territory Surface Tile
              SDL_Rect sourceQuad;
                //Replace this with logic based on territory.terrain.surfaceMap[i][j];
                sourceQuad.x=(int)(territory.terrain.localMap[i][j]*4)%3*11;
                sourceQuad.y=territory.terrain.biomeMap[(player.xTotal+i-25)/1000][(player.yTotal+j-25)/1000]*11;
                sourceQuad.w=11;
                sourceQuad.h=11;
              //Take Renderquad from current i and j numbers
              //Height is in Intervals of 100
                SDL_Rect renderQuad;
                renderQuad.w=tileScale*11;
                renderQuad.h=tileScale*11;
                renderQuad.x=territory.terrain.worldWidth/2+tileScale*5*((j-25)-(i-25)-1);
                renderQuad.y=territory.terrain.worldHeight/2-tileScale*5+3*tileScale*((j-25)+(i-25))-((int)territory.terrain.localMap[i][j]-(int)territory.terrain.localMap[24][24])*5*tileScale;
                //Render
                //Render the Vegetation on the Map
                SDL_RenderCopy( gRenderer, mTexture, &sourceQuad, &renderQuad);
                renderVegetation(territory, gRenderer, player, i, j, tileScale);
          }
        }
  }
