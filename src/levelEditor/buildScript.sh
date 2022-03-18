#!/bin/bash
clang -c -std=gnu99 -I$(pwd)/include/flecs -I$(pwd)/include -g -o build/flecs.o src/flecs.c

clang++ -c -std=c++11 -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o build/input.o src/input.cpp -Wno-expansion-to-defined

clang++ -c -std=c++11 -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o build/debug_display.o src/debug_display.cpp -Wno-expansion-to-defined

clang++ -c -std=c++11 -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o build/camera.o src/camera.cpp -Wno-expansion-to-defined

clang++ -c -std=c++11 -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o build/shapeTransformations.o src/shapeTransformations.cpp -Wno-expansion-to-defined

clang++ -c -std=c++11  -I$(pwd)/include -I$(pwd)/include/SDL2 -g -o build/main.o src/levelEditor/levelEditorMain.cpp -Wno-expansion-to-defined

clang++ -std=c++11  -L$(pwd)/lib -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,$(pwd)/lib build/flecs.o build/input.o  build/debug_display.o build/camera.o build/shapeTransformations.o build/main.o -g -o ed