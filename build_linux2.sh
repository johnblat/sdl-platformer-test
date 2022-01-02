#!/usr/bin/bash

clang -c -std=gnu99 -I/home/frog/repos/sdl-repos/sdl-platformer-test/include -I/home/frog/repos/sdl-repos/sdl-platformer-test/include/SDL2 -I/home/frog/repos/sdl-repos/sdl-platformer-test/include/flecs  -o flecs.o src/flecs.c 

clang++ -c  -std=c++11 -Wno-expansion-to-defined -I/home/frog/repos/sdl-repos/sdl-platformer-test/include -I/home/frog/repos/sdl-repos/sdl-platformer-test/include/SDL2 -I/home/frog/repos/sdl-repos/sdl-platformer-test/include/flecs  -o animationProcessing.o src/animationProcessing.cpp 

clang++ -c  -std=c++11 -Wno-expansion-to-defined -I/home/frog/repos/sdl-repos/sdl-platformer-test/include -I/home/frog/repos/sdl-repos/sdl-platformer-test/include/SDL2 -I/home/frog/repos/sdl-repos/sdl-platformer-test/include/flecs  -o spriteSheetsProcessing.o src/animationProcessing.cpp 

clang++ -c  -std=c++11 -Wno-expansion-to-defined -I/home/frog/repos/sdl-repos/sdl-platformer-test/include -I/home/frog/repos/sdl-repos/sdl-platformer-test/include/SDL2 -I/home/frog/repos/sdl-repos/sdl-platformer-test/include/flecs  -o game_main.o src/animationProcessing.cpp 

clang++ -L/home/frog/repos/sdl-repos/sdl-platformer-test/lib -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,/home/frog/repos/sdl-repos/sdl-platformer-test/lib flecs.o animationProcessing.o spriteSheetsProcessing.o game_main.o -g -o app