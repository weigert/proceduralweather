//Player Handling Class for Close Viewmode
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

class Player {
 public:
   //Global Division = 1000 Blocks
   int xGlobal = 15;
   int yGlobal = 20;
   //Local Division = 10 Blocks
	 int xRegion = 0; //73
   int yRegion = 0; //30
   //Local Division = 10
   int xLocal = 0;
   int yLocal = 0;
   //Total Division = 10'000 (or 1 hour walktime, with .5)
   int xTotal = 1000;
   int yTotal = 1000;

   void changePos(SDL_Event e);
};

void Player::changePos(SDL_Event e){
  switch( e.key.keysym.sym){
    case SDLK_DOWN:
      if(yLocal < 9){
        yLocal += 1;
      }
      else{
        if(yRegion < 99){
          yLocal = 0;
          yRegion += 1;
        }
        else{
          if(yGlobal < 99){
            yLocal = 0;
            yRegion = 0;
            yGlobal += 1;
          }
        }
      }
    break;

    case SDLK_UP:
      if(yLocal > 0){
        yLocal -= 1;
      }
      else{
        if(yRegion > 0){
          yLocal = 9;
          yRegion -= 1;
        }
        else{
          if(yGlobal > 0){
            yLocal = 9;
            yRegion = 99;
            yGlobal -= 1;
          }
        }
      }
    break;

    case SDLK_RIGHT:
      if(xLocal > 0){
        xLocal -= 1;
      }
      else{
        if(xRegion > 0){
          xLocal = 9;
          xRegion -= 1;
        }
        else{
          if(xGlobal > 0){
            xLocal = 9;
            xRegion = 99;
            xGlobal -= 1;
          }
        }
      }
    break;

    case SDLK_LEFT:
      if(xLocal < 9){
        xLocal += 1;
      }
      else{
        if(xRegion < 99){
          xLocal = 0;
          xRegion += 1;
        }
        else{
          if(xGlobal < 99){
            xLocal = 0;
            xRegion = 0;
            xGlobal += 1;
          }
        }
      }
    break;
  }
  //Calculate the Overall Position
  xTotal = xGlobal*1000+xRegion*10+xLocal;
  yTotal = yGlobal*1000+yRegion*10+yLocal;
}
