#pragma once

#include "circle.h"
#include "flecs.h"
#include "ints.h"
#include "position.h"
#include "ray2d.h"
#include "states.h"
#include <vector>
#include "velocity.h"




bool collisions_Ray2d_intersects_line_segment(Ray2d ray, Position p1, Position p2, float &distanceFromRayOrigin, SensorType sensorType);

bool collisions_point_intersect_circle(v2d p, Circle circle);

bool collisions_line_segment_intersect_circle(v2d p1, v2d p2, Circle circle);