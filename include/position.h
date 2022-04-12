#ifndef posh
#define posh
#include "v2d.h"
#include <vector>
#include <SDL2/SDL.h>

typedef v2d Position;


struct Angle {
    float rads;
};


struct PlatformVertex {
    float x, y;
};


struct PlatformVertices {
    std::vector<PlatformVertex> vals;
    SDL_Color color;
};


#define degToRads(deg)(deg * (3.14/180))


#endif