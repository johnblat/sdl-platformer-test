#include "flecs.h"
#include "input.h"


void savePlatformVertices(flecs::world &ecs);
void saveSystem(flecs::iter &it, Input *inputs);
void loadInputSystem(flecs::iter &it, Input *inputs);