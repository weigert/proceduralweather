OBJS = territory.cpp
CC = g++ -std=c++11
COMPILER_FLAGS = -Wall
LINKER_FLAGS = -lSDL2 -I/usr/local/include -L/usr/local/lib -lnoise -lSDL2_image -lSDL2_ttf
OBJ_NAME = territory
all: $(OBJS)
			$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
