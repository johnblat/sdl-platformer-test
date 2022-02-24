CXX = clang++
CC = clang

CWD = $(shell pwd)
CFLAGS = -std=gnu99
CXXFLAGS = -std=c++11 -Wno-expansion-to-defined

CSOURCES = src/flecs.c

CXXSOURCES = src/animationProcessing.cpp src/spriteSheetsProcessing.cpp src/game_main.cpp src/input.cpp src/movement.cpp src/collisions.cpp src/debug_display.cpp src/camera.cpp


LFLAGS = -L$(CWD)/lib
IFLAGS = -I$(CWD)/include -I$(CWD)/include/SDL2 -I$(CWD)/include/flecs 

LIBS = -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,$(CWD)/lib

BUILD_PATH = build

CXXFILENAMES := $(notdir $(CXXSOURCES))
CFILENAMES := $(notdir $(CSOURCES))

CXXOBJ := $(CXXFILENAMES:%.cpp=build/%.o)
COBJ := $(CFILENAMES:%.c=build/%.o)

OBJ := $(COBJ) $(CXXOBJ)


all: $(COBJ) $(CXXOBJ)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o app

build/%.o: src/%.c
	$(CC) $(CFLAGS) $(IFLAGS) -c -g $^ -o $@

build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -g $^ -o $@

.PHONY: clean 
clean:
	rm -f build/*.o 