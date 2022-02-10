#ifndef movementh
#define movementh

#include "flecs.h"
#include "velocity.h"
#include "position.h"
#include "input.h"
#include "ray2d.h"
#include "jbInts.h"
#include <vector>


void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions);

void InputVelocitySetterSystem(flecs::iter &it, Velocity *velocities, Input *inputs);

void gravitySystem(flecs::iter &it, Velocity *velocities);


#endif