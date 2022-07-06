#pragma once

#include "flecs.h"
#include "position.h"
#include "ray2d.h"
#include "states.h"
#include <vector>
#include "velocity.h"

void collisions_Sensors_wall_sensors_set_distance_from_velocity_System(flecs::iter &it, SensorCollection *sensorCollections, Velocity *velocities, GroundMode *groundModes);

void collisions_Sensors_wall_update_Position_System(flecs::iter &it, Position *positions, SensorCollection *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles);

void collisions_Sensors_wall_set_height_from_Angle_System(flecs::iter &it, SensorCollection *sensorCollections, Angle *angles);

void collisions_GroundMode_update_from_Angle_System(flecs::iter &it, GroundMode *groundModes, Angle *angles);

void 
collisions_floor_sensors_intersect_platform_paths_set_result_System(
    flecs::iter &it, 
    Position *positions, 
    Velocity *velocities, 
    SensorCollection *sensor_collections, 
    GroundMode *groundModes, 
    StateCurrPrev *states, 
    Angle *angles,
    CollisionResultPlatformPathFloorSensor *floor_platform_collision_results
);


void 
collisions_position_rotation_align_based_on_collision_result_System(
    flecs::iter &it, 
    Position *positions,
    Angle *angles,
    SensorCollection *sensor_collections,
    GroundMode *ground_modes,
    StateCurrPrev *states,
    CollisionResultPlatformPathFloorSensor *floor_platform_collision_results
);
