# proceduralweather

### Installation:
This was originally compiled using gcc on ubuntu. To compile it with a similar setup, just issue the command >make all. If you're not sure, look at the makefile and make sure the header files are included correctly for your OS. Also you need gcc.
Then just use the executable and you should be able to generate the maps yourself. 

### Changing the seed:
You can change the seed in worldgen.h to get a different result. Don't forget to recompile.

### Changing the map-overlay:
You can change the map-overlay (climate map view) by changing the number in line 107 in territory.cpp.
      0 - Windmap
      1 - Cloudmap
      2 - Rainmap
      3 - Humditymap
      4 - Tempmap
      5 - Average Windmap
      6 - Average Cloudmap
      7 - Average Rainmap
      8 - Average Humiditymap
      9 - Average Tempmap

For everything else you'll have to look at the code. Written in C++ by Nicholas McDonald, 2018.
