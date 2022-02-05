#ifndef movementh
#define movementh

#include "flecs.h"
#include "velocity.h"
#include "position.h"
#include "input.h"
#include "jbInts.h"


void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions);

void InputVelocitySetterSystem(flecs::iter &it, Velocity *velocities, Input *inputs);

#endif