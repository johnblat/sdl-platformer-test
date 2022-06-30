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

const float SENSOR_FLOOR_AIR_DISTANCE = 16.0f;
const float SENSOR_FLOOR_GROUND_DISTANCE = 30.0f;
const float HALF_PLAYER_HEIGHT = 19.0f;


typedef struct Sensors Sensors;
struct Sensors {
    Ray2d rays[6];
};

#endif