#pragma once

#include "flecs.h"
#include "position.h"
#include "ray2d.h"
#include "states.h"
#include <vector>
#include "velocity.h"

void collisions_Sensors_wall_sensors_set_distance_from_velocity_System(flecs::iter &it, Sensors *sensorCollections, Velocity *velocities, GroundMode *groundModes);

void collisions_Sensors_wall_update_Position_System(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles);

void collisions_Sensors_wall_set_height_from_Angle_System(flecs::iter &it, Sensors *sensorCollections, Angle *angles);

void collisions_GroundMode_update_from_Angle_System(flecs::iter &it, GroundMode *groundModes, Angle *angles);

void collisions_Sensors_PlatformPaths_update_Position_System(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode* groundModes, StateCurrPrev *states, Angle *angles );
