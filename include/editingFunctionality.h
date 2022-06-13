#pragma once

#include "flecs.h"
#include "mouseState.h"


void EndEditingSelectedPlatformVertices(flecs::world &ecs);
void createAndSelectPlatformVerticesEntity(flecs::world &ecs, PlatformVertexCollection pvc);
void EditPlatformVerticesAddVertexAtMousePositionOnSelectedSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates);
void DeselectInputSystem(flecs::iter &it, Input *inputs);
void SelectPlatformVertexCollectionOnMouseClick(flecs::iter &it, Position *positions, PlatformVertexCollection *pvcs);

void SelectedPlatformVertexCollectionDeletionSystem(flecs::iter &it, Position *positions, PlatformVertexCollection *pvcs, SelectedForEditing *s);

