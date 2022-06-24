#pragma once

#include <SDL2/SDL.h>
#include "animation.h"
#include <vector>
#include "position.h"
#include "flecs.h"

extern SDL_Renderer *gRenderer;

void render_diamond(Position centerPoint, SDL_Color color);

void renderAnimatedSprite(float x, float y, AnimatedSprite animatedSprite);

void render_poly_line(Position offsetPosition, std::vector<Position> points, SDL_Color color);

void render_frame_start_System(flecs::iter &it);

void render_end_frame_System(flecs::iter &it);

void render_line(Position p1, Position p2, SDL_Color color);