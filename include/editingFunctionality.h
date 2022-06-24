#pragma once

#include "flecs.h"
#include "mouseState.h"

namespace EditMode {
    struct PlatformPathCreateMode{};
    struct PlatformPathSelectMode{};
    struct PlatformPathNodeAppendMode{};
    struct PlatformPathNodeSelectMode{};
    struct PlatformPathNodeMoveMode{};
}



static void 
internal_PlatformPath_entity_create_and_init(flecs::world &ecs, PlatformPath platformPath);

void 
ed_PlatformPath_create_entity_on_click_System(flecs::iter &it, MouseState *mouseStates);

void
ed_PlatformPath_select_on_click_System(flecs::iter &it, MouseState *mouseStates);

void 
ed_PlatformPath_destruct_selected_on_delete_button_release_System(flecs::iter &it, Position *positions, PlatformPath *platformPaths, SelectedForEditing *s);

void 
ed_PlatformPath_node_append_to_selected_on_click_System(flecs::iter &it, Input *inputs, MouseState *mouseStates);

void 
ed_PlatformPath_node_select_on_click_System(flecs::iter &it, MouseState *mouseStates);

void 
ed_PlatformPath_node_move_on_drag_System(flecs::iter &it, MouseState *mouseStates);

void 
ed_SelectedForEditing_tag_remove_all_and_set_default_EditMode_on_deselect_button_release_System(flecs::iter &it, Input *inputs);

void 
ed_EditMode_change_depending_on_Input_release(flecs::iter &it, Input *inputs);

void ed_PlatformPaths_remove_all_on_right_mouse_click_System(flecs::world &ecs, MouseState &mouseState);


