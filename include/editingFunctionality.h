#pragma once

#include "flecs.h"
#include "mouseState.h"

namespace EditMode {
    struct CreatePncMode{};
    struct SelectPncMode{};
    struct AppendNodeMode{};
    struct SelectNodeMode{};
    struct MoveNodeMode{};
}



extern flecs::entity editorEntity;

static void EndEditingSelectedPlatformNode(flecs::world &ecs);
static void createAndSelectPlatformNodeEntity(flecs::world &ecs, PlatformNodeCollection pnc);

void AppendNodeToSelectedPncSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates);
void DeselectInputSystem(flecs::iter &it, Input *inputs);
void SelectPlatformNodeCollectionOnMouseClick(flecs::iter &it, Position *positions, PlatformNodeCollection *pncs);

void DeleteSelectedPncSystem(flecs::iter &it, Position *positions, PlatformNodeCollection *pncs, SelectedForEditing *s);

void SelectNodeSystem(flecs::iter &it, SelectedForEditing *s, Position *positions, PlatformNodeCollection *pncs);


void selectPncSystem(flecs::iter &it, MouseState *mouseStates);

// 

