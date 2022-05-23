CXX = clang++
CC = clang

BUILD_DIR := .build

DEPDIR := .deps
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.cpp.d

CFLAGS = -std=gnu99
CXXFLAGS = -std=c++11 -Wno-expansion-to-defined

SHARED_C_SOURCES = shared/flecs.c
SHARED_C_OBJ_FILES := $(SHARED_C_SOURCES:%.c=$(BUILD_DIR)/%.c.o)

SHARED_CXX_SOURCES = $(wildcard src/shared/*.cpp)
SHARED_CXX_OBJ_FILES := $(SHARED_CXX_SOURCES:src/%.cpp=$(BUILD_DIR)/%.cpp.o)
$(info SHARED FILES)
$(info =============)
$(info Shared source files: $(SHARED_CXX_SOURCES))
$(info )
$(info Shared obj files: $(SHARED_CXX_OBJ_FILES))
$(info =============)

GAME_CXX_SOURCES = $(wildcard src/game/*.cpp)
GAME_CXX_OBJ_FILES := $(GAME_CXX_SOURCES:src/%.cpp=$(BUILD_DIR)/%.cpp.o)
$(info GAME FILES)
$(info =============)
$(info game src files: $(GAME_CXX_SOURCES))
$(info )
$(info game obj files: $(GAME_CXX_OBJ_FILES))
$(info =============)


ED_CXX_SOURCES = $(wildcard src/levelEditor/*.cpp)
ED_CXX_OBJ_FILES := $(ED_CXX_SOURCES:src/%.cpp=$(BUILD_DIR)/%.cpp.o)
$(info ED FILES)
$(info =============)
$(info ed src files: $(ED_CXX_SOURCES))
$(info )
$(info ed obj files: $(ED_CXX_OBJ_FILES))
$(info =============)


LFLAGS = -Llib
IFLAGS = -Iinclude -Iinclude/SDL2 -Iinclude/flecs 

LIBS = -lSDL2 -lSDL2main -lSDL2_image -Wl,-rpath,lib





game: $(GAME_CXX_OBJ_FILES) $(SHARED_CXX_OBJ_FILES) $(SHARED_C_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o $@

ed: $(ED_CXX_OBJ_FILES) $(SHARED_C_OBJ_FILES) $(SHARED_CXX_OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(LFLAGS) $(LIBS) -g $^ -o $@



$(BUILD_DIR)/%.c.o: src/%.c 
	$(CC) $(CFLAGS) $(IFLAGS) -c -g $^ -o $@

$(BUILD_DIR)/%.cpp.o: src/%.cpp 
$(BUILD_DIR)/%.cpp.o: src/%.cpp $(DEPDIR)/%.cpp.d | $(DEPDIR) $(BUILD_DIR)
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) $(IFLAGS) -c -g $< -o $@

# needed directories for build process
$(DEPDIR):
	mkdir -p $@
	mkdir -p $@/game
	mkdir -p $@/shared
	mkdir -p $@/levelEditor

$(BUILD_DIR):
	mkdir -p $@
	mkdir -p $@/game
	mkdir -p $@/shared
	mkdir -p $@/levelEditor



DEPFILES := $(GAME_CXX_SOURCES:src/%.cpp=$(DEPDIR)/%.cpp.d)
DEPFILES += $(SHARED_CXX_SOURCES:src/%.cpp=$(DEPDIR)/%.cpp.d)
DEPFILES += $(ED_CXX_SOURCES:src/%.cpp=$(DEPDIR)/%.cpp.d)

$(DEPFILES):

include $(wildcard $(DEPFILES))

# #.PHONY: clean 
clean:
	rm -f src/game/*.o
	rm -f src/shared/*.o
	rm -f src/levelEditor/*.o 
	rm -f build/game/*.o
	rm -f build/shared/*.o
	rm -f build/levelEditor/*.o