#pragma once

#include "circle.h"
#include "flecs.h"
#include "ints.h"
#include "position.h"
#include "ray2d.h"
#include "states.h"
#include <vector>
#include "velocity.h"


struct CollisionResultRay2dIntersectLine{
    bool  did_intersect;
    v2d   p_world_intersection;
    float distance_from_ray_origin;
    v2d   p1_intersecting_line;
    v2d   p2_intersecting_line;
};

bool collisions_Ray2d_intersects_line_segment(Ray2d ray_sensor, SensorType sensorType, Position p1_line, Position p2_line, float &distance_from_ray_sensor_origin, v2d &p_intersection);

CollisionResultRay2dIntersectLine collisions_Ray2d_intersects_line_segment_result(Ray2d ray_sensor, SensorType sensor_type, Position p1_line, Position p2_line);


CollisionResultRay2dIntersectLine collisions_Ray2d_rotated_intersects_line_segment_result(Ray2d ray_sensor_world, SensorType sensor_type, float sensor_rotation_in_rads, v2d p_origin_of_rotation, Position p1_line, Position p2_line);

bool collisions_point_intersect_circle(v2d p, Circle circle);

bool collisions_line_segment_intersect_circle(v2d p1, v2d p2, Circle circle);