#ifndef renderh
#define renderh

#include <SDL2/SDL.h>
#include "animation.h"
#include <vector>
#include "position.h"

extern SDL_Renderer *gRenderer;

void renderAnimatedSprite(float x, float y, AnimatedSprite animatedSprite);

void renderPolyLineInCamera(Position offsetPosition, std::vector<Position> points, SDL_Color color);

#endif