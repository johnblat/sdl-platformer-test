#pragma once

#include <SDL2/SDL.h>
#include "animation.h"
#include <vector>
#include "position.h"
#include "flecs.h"

extern SDL_Renderer *gRenderer;

void renderDiamond(Position centerPoint);

void renderAnimatedSprite(float x, float y, AnimatedSprite animatedSprite);

void renderPolyLineInCamera(Position offsetPosition, std::vector<Position> points, SDL_Color color);

void renderFrameStartSystem(flecs::iter &it);

void renderEndFrameSystem(flecs::iter &it);
