#include "flecs.h"
#include "input.h"


void save_PlatformPaths(flecs::world &ecs);
void save_PlatformPaths_on_save_button_release_System(flecs::iter &it, Input *inputs);
void load_PlatformPaths_on_load_button_release_System(flecs::iter &it, Input *inputs);
void 
load_PlatformPaths(flecs::world &ecs);
