#pragma once

#include "mouseState.h"
#include "flecs.h"

void mouseStateSetterSystem(flecs::iter &it, MouseState *mouseStates);
void mouesStatePlatformNodeRemoveAll(flecs::world &ecs, MouseState &mouseState);
