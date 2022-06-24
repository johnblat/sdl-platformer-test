#pragma once

#include <string>
#include "spriteSheets.h"
#include "flecs.h"
#include "position.h"
#include "ParallaxSprite.h"


void render_ParallaxSprite_System(flecs::iter &it, Position *positions, ParallaxSprite *paralllaxSprites);