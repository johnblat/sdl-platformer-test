#ifndef renderh
#define renderh

#include <SDL2/SDL.h>
#include "animation.h"

extern SDL_Renderer *gRenderer;

void renderAnimatedSprite(float x, float y, AnimatedSprite animatedSprite);

#endif