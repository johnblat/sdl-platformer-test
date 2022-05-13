#pragma once

#include "mouseState.h"
#include "flecs.h"


void mouseStateSetter(MouseState &mouseState);
void mouesStatePlatformVerticesRemoveAll(flecs::world &ecs, MouseState &mouseState);
