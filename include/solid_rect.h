#ifndef solid_rect_h
#define solid_rect_h

#include <SDL2/SDL.h>

typedef struct SolidRect SolidRect;
struct SolidRect {
    SDL_Rect rect;
    float rotation;
    SDL_Color color;
};

#endif