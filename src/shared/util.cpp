#include "util.h"
#include "velocity.h"

inline float rads2deg(float rads){
    return rads * 180 / 3.14;
}

GroundMode whichGroundMode(float angleInRads){
    float angleInDegrees = rads2deg(angleInRads);

    if(inRange(angleInDegrees, 46.0f, 134.0f)){
        return RIGHT_WALL_GM;
    }
    if(inRange(angleInDegrees, 135.0f, 225.0f)){
        return CEILING_GM;
    }
    if(inRange(angleInDegrees, 226.0f, 314.0f)){
        return LEFT_WALL_GM;
    }

    return FLOOR_GM;
    
}


bool inRange(float val, float start, float end){
    if(end > start){
        swapValues(end, start, float);
    }

    if(val >= start && val <= end){
        return true;
    }

    return false;

}
