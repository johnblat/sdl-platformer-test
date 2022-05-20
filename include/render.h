#pragma once

#include <SDL2/SDL.h>
#include "animation.h"
#include <vector>
#include "position.h"
#include "flecs.h"

extern SDL_Renderer *gRenderer;

void renderDiamondInCamera(Position centerPoint, SDL_Color color);

void renderAnimatedSprite(float x, float y, AnimatedSprite animatedSprite);

void renderPolyLineInCamera(Position offsetPosition, std::vector<Position> points, SDL_Color color);

void renderFrameStartSystem(flecs::iter &it);

void renderEndFrameSystem(flecs::iter &it);

void renderLineInCamera(Position p1, Position p2, SDL_Color color);