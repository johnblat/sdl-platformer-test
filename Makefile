CXX = clang++
CC = clang

CFLAGS = -std=gnu99
CXXFLAGS = -std=c++11 -Wno-expansion-to-defined

SHARED_C_SOURCES = src/shared/flecs.c
SHARED_C_OBJ_FILES := $(SHARED_C_SOURCES:%.c=%.c.o)

SHARED_CXX_SOURCES =    src/shared/input.cpp   src/shared/debug_display.cpp src/shared/shapeTransformations.cpp src/shared/camera.cpp
SHARED_CXX_OBJ_FILES := $(SHARED_CXX_SOURCES:%.cpp=%.cpp.o)

GAME_CXX_SOURCES = src/game/game_main.cpp  src/game/collisions.cpp src/game/movement.cpp src/game/animationProcessing.cpp src/game/spriteSheetsProcessing.cpp src/game/stateProcessing.cpp
GAME_CXX_OBJ_FILES := $(GAME_CXX_SOURCES:%.cpp=%.cpp.o)

ED_CXX_SOURCES = src/levelEditor/platformLevelEditor.cpp
ED_CXX_OBJ_FILES := $(ED_CXX_SOURCES:%.cpp=%.cpp.o)

LFLAGS = -Llib
IFLAGS = -Iinclude -Iinclude/SDL2 -Iinclude/flecs 

LIBS = -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,lib


game: $(GAME_CXX_OBJ_FILES) $(SHARED_CXX_OBJ_FILES) $(SHARED_C_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o $@

ed: $(ED_CXX_OBJ_FILES) $(SHARED_C_OBJ_FILES) $(SHARED_CXX_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o $@

%.c.o: %.c
	$(CC) $(CFLAGS) $(IFLAGS) -c -g $^ -o $@

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -g $^ -o $@



# #.PHONY: clean 
clean:
	rm -f src/game/*.o
	rm -f src/shared/*.o
	rm -f src/levelEditor/*.o 