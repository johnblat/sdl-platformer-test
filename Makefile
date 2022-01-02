CXX = clang++
CC = clang

CWD = $(shell pwd)
CFLAGS = -std=gnu99
CXXFLAGS = -std=c++11 -Wno-expansion-to-defined

CSOURCES = src/flecs.c

CXXSOURCES = src/animationProcessing.cpp src/spriteSheetsProcessing.cpp src/game_main.cpp 


LFLAGS = -L$(CWD)/lib
IFLAGS = -I$(CWD)/include -I$(CWD)/include/SDL2 -I$(CWD)/include/flecs 

LIBS = -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,$(CWD)/lib

BUILD_PATH = build

CXXFILENAMES := $(notdir $(CXXSOURCES))
CFILENAMES := $(notdir $(CSOURCES))

CXXOBJ := $(CXXSOURCES:%.cpp=%.o)
COBJ := $(CSOURCES:%.c=%.o)

OBJ := $(COBJ) $(CXXOBJ)

BUILTOBJ := $(addprefix $(BUILD_PATH)/,$(notdir $(OBJ)))



app:  $(COBJ) $(CXXOBJ)
	$(CXX) $(LFLAGS) $(LIBS) $^ -g -o $@
	
%.o: %.c
	$(CC) -c $(CFLAGS) $(IFLAGS) -o $@ $< 

%.o: %.cpp
	$(CXX) -c  $(CXXFLAGS) $(IFLAGS) -o  $@ $< 

clean:
	rm -f src/*.o 