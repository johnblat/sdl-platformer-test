#pragma once

#include "flecs.h"
#include "mouseState.h"


void EndEditingSelectedPlatformVertices(flecs::world &ecs);
void createAndSelectPlatformVerticesEntity(flecs::world &ecs, PlatformVertices pvs);
void EditPlatformVerticesAddVertexAtMousePositionOnSelected(flecs::world &ecs, MouseState &mouseState);