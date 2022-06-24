#pragma once

#include "states.h"
#include "position.h"
#include "flecs.h"
#include "velocity.h"
#include <vector>
#include "ray2d.h"
#include "ints.h"



void collisions_Sensors_PlatformPaths_update_Position_System(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode* groundModes, StateCurrPrev *states, Angle *angles );

bool collisions_Ray2d_intersects_line_segment(Ray2d ray, Position p1, Position p2, float &distanceFromRayOrigin, SensorType sensorType);

bool collisions_point_intersects_point_with_tolerance(v2d p1, v2d p2, float tolerance);

bool collisions_horizontal_Ray2d_intersect_line_segment(Ray2d ray, v2d linePoint1, v2d linePoint2);

bool collisions_vertical_Ray2d_intersect_line_segment(Ray2d ray, v2d linePoint1, v2d linePoint2);

bool collisions_point_intersect_line_segment_with_tolerance(v2d linePoint1, v2d linePoint2, v2d p, float tolerance );
