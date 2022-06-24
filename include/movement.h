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


void movement_apply_velocity_to_position_System(flecs::iter &it, Velocity *velocities, Position *positions);

void movement_GrounSpeed_Velocity_update_from_Input_and_Phyics_System(flecs::iter &it, Velocity *velocities, GroundSpeed *groundSpeeds, Input *inputs, StateCurrPrev *states, Angle *angles);

void movement_velocity_apply_gravity_System(flecs::iter &it, Velocity *velocities, StateCurrPrev *states);


#endif
