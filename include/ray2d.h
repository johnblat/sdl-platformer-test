#ifndef ray_h
#define ray_h

#include "position.h"


// always points downwards
typedef struct Ray2d Ray2d;
struct Ray2d {
    Position startingPosition;
    float distance; 
};

#endif