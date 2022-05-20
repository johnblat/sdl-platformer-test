#ifndef movementh
#define movementh

#include "flecs.h"
#include "velocity.h"
#include "position.h"
#include "input.h"
#include "ray2d.h"
#include "ints.h"
#include <vector>
#include "states.h"


void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions);

void InputVelocitySetterSystem(flecs::iter &it, Velocity *velocities, GroundSpeed *groundSpeeds, Input *inputs, StateCurrPrev *states, Angle *angles);

void gravitySystem(flecs::iter &it, Velocity *velocities, StateCurrPrev *states);


#endif
