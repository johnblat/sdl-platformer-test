#ifndef ray_h
#define ray_h

#include "position.h"


// always points downwards
typedef struct Ray2d Ray2d;
struct Ray2d {
    Position startingPosition;
    float distance; 
};

enum SensorType {
    LF_SENSOR, // Left Floor
    RF_SENSOR, // Right Floor
    LW_SENSOR, // Left Wall
    RW_SENSOR, // Right Wall
    LC_SENSOR, // Left Ceiling
    RC_SENSOR // Right Ceiling
};


typedef struct Sensors Sensors;
struct Sensors {
    Ray2d rays[6];
};

#endif