#ifndef collisions_h
#define collisions_h

#include "states.h"
#include "position.h"
#include "flecs.h"
#include "velocity.h"
#include <vector>
#include "ray2d.h"
#include "ints.h"



void ray2dPvsCollisionSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, StateCurrPrev *states, Angle *angles );

bool ray2dIntersectLineSegment(Ray2d ray, Position p1, Position p2, float &distanceFromRayOrigin);

#endif