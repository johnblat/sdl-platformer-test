#pragma once

#include "mouseState.h"
#include "flecs.h"

void MouseState_update_System(flecs::iter &it, MouseState *mouseStates);
void ed_PlatformPaths_remove_all_on_right_mouse_click_System(flecs::world &ecs, MouseState &mouseState);
