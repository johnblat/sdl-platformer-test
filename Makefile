CXX = clang++
CC = clang

CFLAGS = -std=gnu99
CXXFLAGS = -std=c++11 -Wno-expansion-to-defined

SHARED_C_SOURCES = src/shared/flecs.c
SHARED_C_OBJ_FILES := $(SHARED_C_SOURCES:%.c=build/%.c.o)

SHARED_CXX_SOURCES =    src/shared/input.cpp   src/shared/debug_display.cpp src/shared/shapeTransformations.cpp src/shared/camera.cpp src/shared/resourceLoading.cpp src/shared/eventHandling.cpp src/shared/timestep.cpp src/shared/render.cpp src/shared/mouseStateProcessing.cpp src/shared/loadSave.cpp src/shared/editingFunctionality.cpp

SHARED_CXX_OBJ_FILES := $(SHARED_CXX_SOURCES:%.cpp=build/%.cpp.o)

GAME_CXX_SOURCES = src/game/game_main.cpp  src/game/collisions.cpp src/game/movement.cpp src/game/animationProcessing.cpp src/game/spriteSheetsProcessing.cpp src/game/stateProcessing.cpp src/game/parallaxSpriteProcessing.cpp 

GAME_CXX_OBJ_FILES := $(GAME_CXX_SOURCES:%.cpp=build/%.cpp.o)

ED_CXX_SOURCES = src/levelEditor/platformLevelEditor.cpp 
ED_CXX_OBJ_FILES := $(ED_CXX_SOURCES:%.cpp=build/%.cpp.o)

LFLAGS = -Llib
IFLAGS = -Iinclude -Iinclude/SDL2 -Iinclude/flecs 

LIBS = -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,lib

# test
BASE = build/src/game/game_main
FILE = $(BASE).cpp.o 
RESULT = $(subst build/,,$(BASE).cpp)
$(info $(RESULT))
# end test

game: $(GAME_CXX_OBJ_FILES) $(SHARED_CXX_OBJ_FILES) $(SHARED_C_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o $@

ed: $(ED_CXX_OBJ_FILES) $(SHARED_C_OBJ_FILES) $(SHARED_CXX_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o $@

%.c.o: 
	$(CC) $(CFLAGS) $(IFLAGS) -c -g $(subst .o,,$(subst build/,,$@)) -o $@

%.cpp.o: 
	$(CXX) $(CXXFLAGS) $(IFLAGS) -c -g $(subst .o,,$(subst build/,,$@)) -o $@



# #.PHONY: clean 
clean:
	rm -f src/game/*.o
	rm -f src/shared/*.o
	rm -f src/levelEditor/*.o 
	rm -f build/src/game/*.o
	rm -f build/src/shared/*.o
	rm -f build/src/levelEditor/*.o