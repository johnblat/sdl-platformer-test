#pragma once

#include "flecs.h"
#include "mouseState.h"


void EndEditingSelectedPlatformVertices(flecs::world &ecs);
void createAndSelectPlatformVerticesEntity(flecs::world &ecs, PlatformVertices pvs);
void EditPlatformVerticesAddVertexAtMousePositionOnSelectedSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates);
void DeselectInputSystem(flecs::iter &it, Input *inputs);