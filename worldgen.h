//Worldgen Functions and Classes
#include <noise/noise.h>
#include <iostream>
#include <stdlib.h>
#include "player.h"
#include <SDL2/SDL.h>
#include <time.h>

using namespace noise;

class Climate;
class Terrain;
class World;
class Vegetation;

class Vegetation{
  public:
  //Calculates wether there is a tree or not
  bool getTree(World territory, Player player, int i, int j);
};

class Terrain{
  public:
  int worldDepth = 4000;
  int worldHeight = 1000;
  int worldWidth = 1000;

  //Terrain Parameters
  float depthMap[100][100];
  void genDepth(int seed);

  int biomeMap[100][100];
  void genBiome(Climate climate);

  //Erodes the Landscape for a number of years
  void erode(int seed, Terrain terrain, int years);

  //Local Area (100 Tiles)
  float localMap[50][50];
  void genLocal(int seed, Player player);
};

class Climate {
  public:
  //Curent Climate Maps
  float TempMap[100][100];
  float HumidityMap[100][100];
  bool CloudMap[100][100];
  bool RainMap[100][100];
  float WindMap[100][100];
  double WindDirection[2] = {1,1}; //from 0-1

  //Average Climate Maps
  float AvgRainMap[100][100];
  float AvgWindMap[100][100];
  float AvgCloudMap[100][100];
  float AvgTempMap[100][100];
  float AvgHumidityMap[100][100];

  void init(int day, int seed, Terrain terrain);
  void initTempMap(Terrain terrain);
  void initHumidityMap(Terrain terrain);
  void initCloudMap();
  void initRainMap();

  void calcWind(int day, int seed, Terrain terrain);
  void calcTempMap(Terrain terrain);
  void calcHumidityMap(Terrain terrain);
  void calcDownfallMap();
  void calcWindMap(int day, int seed, Terrain terrain);

  void calcAverage(int seed, Terrain terrain);
};

class World{
  public:
  int xview = 50;
  int yview = 50;
  int seed = 15;
  int day = 0;
  Climate climate;
  Terrain terrain;
  Vegetation vegetation;

  void generate();
  void changePos(SDL_Event e);
};

bool Vegetation::getTree(World territory, Player player, int i, int j){
  //Code to Calculate wether or not we have a tree
  /* Ideally this generates a vegetation map, spitting out
  0 for nothing,
  1 from short grass,
  2 for shrub,
  3 for some herb
  4 for some bush
  5 for some flower
  6 for some tree

  and also gives a number for a variant (3-5 variants of everything per biome)
  every variant could then also have a texture variant if wanted

  For now it only spits out wether or not we have a tree, which it then draws

  We can one piece of vegetation per map

  You could also do this for other objects on the map
  (tents, rocks, other locations) and not place vegetation if there is something present
  */

  //Perlin Noise Module
  module::Perlin perlin;

  perlin.SetOctaveCount(20);
  perlin.SetFrequency(1000);
  perlin.SetPersistence(0.8);

  //Generate the Height Map with Perlin Noise
  float x = (float)(player.xTotal-25+i)/100000;
  float y = (float)(player.yTotal-25+j)/100000;

  //This is not an efficient tree generation method
  //But a reasonable distribution for a grassland area
  srand(x+y);
  int tree = ((int)(1/(perlin.GetValue(x, y, territory.seed+1)+1))*rand()%5)/4;

  return tree;
}

void World::changePos(SDL_Event e){
  switch( e.key.keysym.sym ){
    case SDLK_UP: yview -=50;
      break;
    case SDLK_DOWN: yview +=50;
      break;
    case SDLK_LEFT: xview -= 50;
      break;
    case SDLK_RIGHT: xview += 50;
      break;
    }
}

void World::generate(){
  //Geography
  //Generate and save a heightmap for all Blocks, all Regions
  terrain.genDepth(seed);

  //Erode the Landscape based on iterative average climate
  terrain.erode(seed, terrain, 1);

  //Calculate the climate system of the eroded landscape
  climate.init(day, seed, terrain);
  climate.calcAverage(seed, terrain);

  //Generate the Surface Composition
  terrain.genBiome(climate);
}

void Terrain::genBiome(Climate climate){
  /*
  Determine the Surface Biome:
  0: Water
  1: Sandy Beach
  2: Gravel Beach
  3: Stone Beach Cliffs
  4: Wet Plains (Grassland)
  5: Dry Plains (Shrubland)
  6: Rocky Hills
  7: Tempererate Forest
  8: Boreal Forest
  9: Mountain Tundra
  10: Mountain Peak

  Compare the Parameters and decide what kind of ground we have.
  */
  for(int i = 0; i<100; i++){
    for(int j = 0; j<100; j++){
      //0: Water
      if(depthMap[i][j]<=200){
        biomeMap[i][j] = 0;
      }
      //1: Sandy Beach
      else if(depthMap[i][j]<=204){
        biomeMap[i][j] = 1;
      }
      //2: Gravel Beach
      else if(depthMap[i][j]<210){
        biomeMap[i][j] = 2;
      }
      //3: Stony Beach Cliffs
      else if(depthMap[i][j]<=220){
        biomeMap[i][j] = 3;
      }
      //4: Wet Plains (Grassland)
      //5: Dry Plains (Shrubland)
      else if(depthMap[i][j]<=600){
        if(climate.AvgRainMap[i][j]>=0.02){
          biomeMap[i][j] = 4;
        }
        else{
          biomeMap[i][j] = 5;
        }
      }
      //6: Rocky Hills
      //7: Temperate Forest
      //8: Boreal Forest
      else if(depthMap[i][j]<=1300){
        if(depthMap[i][j]<=1100){
          biomeMap[i][j] = 7;
        }
        else {
          biomeMap[i][j] = 8;
        }
        if(climate.AvgRainMap[i][j]<0.001 && i+rand()%4-2 > 5 && i+rand()%4-2 < 95 && j+rand()%4-2 > 5 && j+rand()%4-2 < 95){
          biomeMap[i][j] = 6;
        }
      }
      else if(depthMap[i][j]<=1500){
        biomeMap[i][j] = 9;
      }
      //Otherwise just Temperate Forest
      else{
        biomeMap[i][j] = 10;
      }
    }
  }
}

void Terrain::erode(int seed, Terrain terrain, int years){
  //Climate Simulation
  Climate average;

  //Simulate the Years
  for(int i = 0; i<years; i++){
    //Initiate the Climate
    average.init(0, seed, terrain);

    //Simulate 1 Year for Average Weather Conditions
    average.calcAverage(seed, terrain);

    //Add Erosion of the Climate after 1 Year
    float erosion = 0;
    for(int j = 0; j<100; j++){
      for(int k=0; k<100; k++){
        erosion = (average.AvgRainMap[j][k] + 0.5*average.AvgWindMap[j][k]);
        depthMap[j][k] = depthMap[j][k] - 5*(depthMap[j][k]/2000) * (1-depthMap[j][k]/2000)*erosion;
      }
    }
  }
}

void Climate::init(int day, int seed, Terrain terrain){
  calcWind(day, seed, terrain);
  initTempMap(terrain);
  initHumidityMap(terrain);
  initRainMap();
  initCloudMap();
}

void Climate::calcAverage(int seed, Terrain terrain){
  //Climate Simulation over n years
  int years = 1;
  int startDay = 0;

  //Initiate average climate maps
  for(int i = 0; i<100; i++){
    for(int j=0; j<100; j++){
      //Start at 0
      AvgRainMap[i][j] = 0;
      AvgWindMap[i][j] = 0;
      AvgCloudMap[i][j] = 0;
      AvgTempMap[i][j] = 0;
      AvgHumidityMap[i][j] = 0;
    }
  }

  //Initiate Simulation at a starting point
  Climate simulation;
  simulation.init(startDay, seed, terrain);

  //Simulate every day for n years
  for(int i = 0; i<years*365; i++){
    //Calculate new Climate
    simulation.calcWind(i, seed, terrain);
    simulation.calcTempMap(terrain);
    simulation.calcHumidityMap(terrain);
    simulation.calcDownfallMap();

    //Average
    for(int j = 0; j<100; j++){
      for(int k = 0; k<100; k++){
        AvgWindMap[j][k] = (AvgWindMap[j][k]*i+simulation.WindMap[j][k])/(i+1);
        AvgRainMap[j][k] = (AvgRainMap[j][k]*i+simulation.RainMap[j][k])/(i+1);
        AvgCloudMap[j][k] = (AvgCloudMap[j][k]*i+simulation.CloudMap[j][k])/(i+1);
        AvgTempMap[j][k] = (AvgTempMap[j][k]*i+simulation.TempMap[j][k])/(i+1);
        AvgHumidityMap[j][k] = (AvgHumidityMap[j][k]*i+simulation.HumidityMap[j][k])/(i+1);
      }
    }
  }
}

void Terrain::genDepth(int seed){
  //Perlin Noise Module

  //Global Depth Map is Fine, unaffected by rivers.
  module::Perlin perlin;

  perlin.SetOctaveCount(12);
  perlin.SetFrequency(2);
  perlin.SetPersistence(0.6);

  //Generate the Perlin Noise World Map
  for(int i = 0; i<100; i++){
    for(int j = 0; j<100; j++){
      //Generate the Height Map with Perlin Noise
      float x = (float)i / 100;
      float y = (float)j / 100;
      depthMap[i][j] = (perlin.GetValue(x, y, seed))/5+0.25;

      //Multiply with the Height Factor
      depthMap[i][j] = depthMap[i][j]*worldDepth;
    }
  }
}

void Terrain::genLocal(int seed, Player player){
  //Perlin Noise Module
  module::Perlin perlin;

  perlin.SetOctaveCount(12);
  perlin.SetFrequency(2);
  perlin.SetPersistence(0.6);

  //Generate the Perlin Noise World Map
  for(int i = 0; i<50; i++){
    for(int j = 0; j<50; j++){
      //Generate the Height Map with Perlin Noise
      float x = (float)(player.xTotal-25+i)/100000;
      float y = (float)(player.yTotal-25+j)/100000;
      localMap[i][j] = (perlin.GetValue(x, y, seed))/5+0.25;
      //Multiply with the Height Factor
      localMap[i][j] = localMap[i][j]*worldDepth;
    }
  }
}

void Climate::calcWind(int day, int seed, Terrain terrain){
  //Perlin Noise Module

  module::Perlin perlin;
  perlin.SetOctaveCount(2);
  perlin.SetFrequency(4);

  float timeInterval = (float)day/365;

  //winddirection shifts every Day
  //One Dimensional Perlin Noise
  WindDirection[1] = (perlin.GetValue(timeInterval, seed, seed));
  WindDirection[2] = (perlin.GetValue(timeInterval, seed+timeInterval, seed));

  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      //Previous Tiles

      int k = i+10*(WindDirection[1]);
      if(k < 0 || k > 99){k = i;};
      int l = j+10*(WindDirection[2]);
      if(l < 0 || l > 99){l = j;};

      WindMap[i][j]=5*(1-(terrain.depthMap[i][j]-terrain.depthMap[k][l])/1000);
    }
  }
}

void Climate::initTempMap(Terrain terrain){
  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      //Sea Temperature
      TempMap[i][j]=0.7; //In Degrees Celsius

      //Add for Height
      if(terrain.depthMap[i][j]>200){
        //In Degrees Celsius
        TempMap[i][j]=1-terrain.depthMap[i][j]/2000;
      }
    }
  }
}

void Climate::initHumidityMap(Terrain terrain){
  //Calculate the Humidity Grid
  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      //Sea Level Temperature
      HumidityMap[i][j]=0; //In Degrees Celsius

      //Humidty Increases for
      if(terrain.depthMap[i][j]<200){
        //In Degrees Celsius
        HumidityMap[i][j]=0.4;
      }
      else{
        HumidityMap[i][j]=0.2;
      }
    }
  }
}

void Climate::initCloudMap(){
  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      CloudMap[i][j]=0;
    }
  }
}

void Climate::initRainMap(){
  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      RainMap[i][j]=0;
    }
  }
}

void Climate::calcHumidityMap(Terrain terrain){
  float oldHumidMap[100][100];
  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      //Old Temperature Map
      oldHumidMap[i][j]=HumidityMap[i][j];
    }
  }

  for(int i=1; i<99; i++){
    for(int j=1; j<99; j++){
      //Get New Map from Wind Direction
      //Indices of Previous Tile
      //Assumption: Wind Blows Despite Obstacles
      int k = i+2*WindMap[i][j]*(WindDirection[1]);
      if(k < 0 || k > 99){k = i;};
      int l = j+2*WindMap[i][j]*(WindDirection[2]);
      if(l < 0 || l > 99){l = j;};
      //Transfer to New Tile
      HumidityMap[i][j]=oldHumidMap[k][l];

      //Average
      HumidityMap[i][j] = (HumidityMap[i-1][j-1]+HumidityMap[i+1][j-1]+HumidityMap[i+1][j+1]+HumidityMap[i-1][j+1] + HumidityMap[i][j] + HumidityMap[i][j+1] + HumidityMap[i][j-1]+HumidityMap[i+1][j]+HumidityMap[i-1][j])/9;

      //We are over a body of water, temperature accelerates
      float addHumidity=0;
      if(CloudMap[i][j]==0){
        addHumidity=0.01;
        if(terrain.depthMap[i][j]<=200){
          addHumidity = 0.05*TempMap[i][j];
        }
      }

      //Raining
      float addRain=0;
      if(RainMap[i][j]==1){
        addRain = -(HumidityMap[i][j])*0.8;
      }

      HumidityMap[i][j]=HumidityMap[i][j]+(HumidityMap[i][j])*addRain+(1-HumidityMap[i][j])*(addHumidity);
      if(HumidityMap[i][j]>1){HumidityMap[i][j]=1;}
      if(HumidityMap[i][j]<0){HumidityMap[i][j]=0;}
    }
  }
}

void Climate::calcTempMap(Terrain terrain){
  float oldTempMap[100][100];
  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      //Old Temperature Map
      oldTempMap[i][j]=TempMap[i][j];
    }
  }

  for(int i=1; i<99; i++){
    for(int j=1; j<99; j++){
      //Get New Map from Wind Direction
      //Indices of Previous Tile
      int k = i+2*WindMap[i][j]*(WindDirection[1]);
      if(k < 0 || k > 99){k = i;};
      int l = j+2*WindMap[i][j]*(WindDirection[2]);
      if(l < 0 || l > 99){l = j;};

      //Transfer to New Tile
      TempMap[i][j]=oldTempMap[k][l];

      //Average
      TempMap[i][j] = (TempMap[i-1][j-1]+TempMap[i+1][j-1]+TempMap[i+1][j+1]+TempMap[i-1][j+1])/4;

      //Various Contributions to the TempMap
      //Rising Air Cools

      float addCool = 0.5*(WindMap[i][j]-5);


      //Sunlight on Surface
      float addSun = 0;
      if(CloudMap[i][j]==0){
        addSun = (1-terrain.depthMap[i][j]/2000)*0.008;
      }

      float addRain = 0;
      if(RainMap[i][j]==1 && TempMap[i][j]>0){
        //Rain Reduces Temperature
        addRain = -0.01;
      }

      //Add Contributions

      TempMap[i][j]=TempMap[i][j]+0.8*(1-TempMap[i][j])*(addSun)+0.6*(TempMap[i][j])*(addRain+addCool);
      if(TempMap[i][j]>1){TempMap[i][j]=1;}
      if(TempMap[i][j]<0){TempMap[i][j]=0;}
    }
  }
}

void Climate::calcDownfallMap(){
  float oldCloudMap[100][100];
  float oldRainMap[100][100];
  for(int i=0; i<100; i++){
    for(int j=0; j<100; j++){
      //Old Temperature Map
      oldRainMap[i][j]=RainMap[i][j];
      oldCloudMap[i][j]=CloudMap[i][j];
      CloudMap[i][j]=0;
      RainMap[i][j]=0;
    }
  }
  for(int i=1; i<99; i++){
    for(int j=1; j<99; j++){
      //Old Coordinates
      int k = i+2*WindMap[i][j]*(WindDirection[1]);
      if(k < 0 || k > 99){k = i;};
      int l = j+2*WindMap[i][j]*(WindDirection[2]);
      if(l < 0 || l > 99){l = j;};

      //Transfer to New Tile
      CloudMap[i][j]=oldCloudMap[k][l];
      RainMap[i][j]=oldRainMap[k][l];

      //Rain Condition
      if(HumidityMap[i][j]>=0.35+0.5*TempMap[i][j]){
        RainMap[i][j]=1;
      }
      else if(HumidityMap[i][j]>=0.3+0.3*TempMap[i][j]){
        CloudMap[i][j]=1;
      }
      else{
        CloudMap[i][j]=0;
        RainMap[i][j]=0;
      }
    }
  }
}
