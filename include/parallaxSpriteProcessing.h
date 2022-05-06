#pragma once

#include <string>
#include "spriteSheets.h"
#include "flecs.h"
#include "position.h"
#include "ParallaxSprite.h"


void renderParallaxSpriteSystem(flecs::iter &it, Position *positions, ParallaxSprite *paralllaxSprites);