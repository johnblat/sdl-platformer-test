CXX = clang++
CC = clang

CFLAGS = -std=gnu99
CXXFLAGS = -std=c++11 -Wno-expansion-to-defined

SHARED_C_SOURCES = shared/flecs.c
SHARED_C_OBJ_FILES := $(SHARED_C_SOURCES:%.c=build/%.c.o)

SHARED_CXX_SOURCES =    shared/input.cpp   shared/debug_display.cpp shared/camera.cpp shared/resourceLoading.cpp shared/eventHandling.cpp shared/timestep.cpp shared/render.cpp shared/mouseStateProcessing.cpp shared/loadSave.cpp shared/editingFunctionality.cpp

SHARED_CXX_OBJ_FILES := $(SHARED_CXX_SOURCES:%.cpp=build/%.cpp.o)

GAME_CXX_SOURCES = game/game_main.cpp  game/collisions.cpp game/movement.cpp game/animationProcessing.cpp game/spriteSheetsProcessing.cpp game/stateProcessing.cpp game/parallaxSpriteProcessing.cpp 

GAME_CXX_OBJ_FILES := $(GAME_CXX_SOURCES:%.cpp=build/%.cpp.o)

ED_CXX_SOURCES = levelEditor/platformLevelEditor.cpp 
ED_CXX_OBJ_FILES := $(ED_CXX_SOURCES:%.cpp=build/%.cpp.o)

LFLAGS = -Llib
IFLAGS = -Iinclude -Iinclude/SDL2 -Iinclude/flecs 

LIBS = -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,lib

# test
BASE = build/src/game/game_main
FILE = $(BASE).cpp.o 
RESULT = $(subst build/,,$(BASE)).cpp
$(info $(RESULT))
# end test

game: $(GAME_CXX_OBJ_FILES) $(SHARED_CXX_OBJ_FILES) $(SHARED_C_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o $@

ed: $(ED_CXX_OBJ_FILES) $(SHARED_C_OBJ_FILES) $(SHARED_CXX_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o $@

build/%.c.o: src/%.c
	$(CC) $(CFLAGS) $(IFLAGS) -c -g $^ -o $@

# .SECONDEXPANSION:
build/%.cpp.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -g $^ -o $@



# #.PHONY: clean 
clean:
	rm -f src/game/*.o
	rm -f src/shared/*.o
	rm -f src/levelEditor/*.o 
	rm -f build/src/game/*.o
	rm -f build/src/shared/*.o
	rm -f build/src/levelEditor/*.o