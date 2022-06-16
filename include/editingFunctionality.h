#pragma once

#include "flecs.h"
#include "mouseState.h"

namespace EditMode {
    struct CreatePvcMode{};
    struct PlatformVertexCollectionSelectMode{};
    struct AddVertexMode{};
    struct VertexSelectMode{};
    struct VertexMoveMode{};
}

extern flecs::entity editorEntity;

void EndEditingSelectedPlatformVertices(flecs::world &ecs);
void createAndSelectPlatformVerticesEntity(flecs::world &ecs, PlatformVertexCollection pvc);
void AddVertexAtMousePositionOnSelectedPvcSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates);
void DeselectInputSystem(flecs::iter &it, Input *inputs);
void SelectPlatformVertexCollectionOnMouseClick(flecs::iter &it, Position *positions, PlatformVertexCollection *pvcs);

void SelectedPlatformVertexCollectionDeletionSystem(flecs::iter &it, Position *positions, PlatformVertexCollection *pvcs, SelectedForEditing *s);

void SelectPlatformVertexOnMouseClick(flecs::iter &it, SelectedForEditing *s, Position *positions, PlatformVertexCollection *pvcs);


void selectPvcAtMousePositionSystem(flecs::iter &it, MouseState *mouseStates);

