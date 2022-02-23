#ifndef collisions_h
#define collisions_h

#include "states.h"
#include "position.h"
#include "flecs.h"
#include "velocity.h"
#include <vector>
#include "ray2d.h"

void ray2dRectangularObjectCollisionSystem(flecs::iter &it, Position *positions, std::vector<Ray2d> *ray2dCollections, Velocity *velocities, State *states );

#endif