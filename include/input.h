#ifndef inputh
#define inputh

#include "jbInts.h"
#include <SDL2/SDL.h>

struct KeyMap {
    char name[16];
    SDL_Scancode sdlScancode;
};

struct KeyboardState{
    u8 *keyStates;
};

#endif