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
    SENSOR_LEFT_FLOOR, // Left Floor
    SENSOR_RIGHT_FLOOR, // Right Floor
    SENSOR_LEFT_WALL, // Left Wall
    SENSOR_RIGHT_WALL, // Right Wall
    SENSOR_LEFT_CEILING, // Left Ceiling
    SENSOR_RIGHT_CEILING // Right Ceiling
};


typedef struct Sensors Sensors;
struct Sensors {
    Ray2d rays[6];
};

#endif