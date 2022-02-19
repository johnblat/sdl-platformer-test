#!/bin/bash

clang -c -std=gnu99 -I$(pwd)/include/flecs -I$(pwd)/include -g -o flecs.o src/flecs.c

clang++ -c -std=c++11 -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o animationProcessing.o src/animationProcessing.cpp -Wno-expansion-to-defined

clang++ -c -std=c++11 -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o spriteSheetsProcessing.o src/spriteSheetsProcessing.cpp -Wno-expansion-to-defined

clang++ -c -std=c++11 -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o movement.o src/movement.cpp -Wno-expansion-to-defined

clang++ -c -std=c++11 -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o input.o src/input.cpp -Wno-expansion-to-defined


# clang++ -c -std=c++11  -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o collisions.o src/collisions.cpp -Wno-expansion-to-defined

clang++ -c -std=c++11  -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o main.o src/game_main.cpp -Wno-expansion-to-defined


clang++ -std=c++11  -L$(pwd)/lib -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,$(pwd)/lib flecs.o input.o animationProcessing.o movement.o spriteSheetsProcessing.o main.o -g -o app

rm -f *.o

# g++ -std=c++11 game/src/game_main.cpp src/animationProcessing.cpp src/spriteSheetsProcessing.cpp src/flecs.c -I$(pwd)/include -I$(pwd)/include/SDL2 -I$(pwd)/include/flecs -L$(pwd)/lib -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,$(pwd)/lib -Wno-expansion-to-defined -g -o app
