#include "util.h"
#include "velocity.h"
#include "position.h"
#include <assert.h>

bool isInRange(float begin, float end, float x){
    if(begin > end){
        swapValues(begin, end, float);
    }
    if(x < begin || x > end){
        return false;
    }
    return true;
}


float getYForXOnLine(Position p1, Position p2, float x){
    if(p2.x < p1.x){
        swapValues(p2, p1, Position);
    }
    if(x < p1.x || x > p2.x){
        assert("getYForXOnLine: X passed must be in the range p1.x to p2.x" && 0);
    }
    if(p1.x == p2.x){
        //printf("p1.x == p2.x");
        return p1.y;
    }
    float m = (p2.y - p1.y)/(p2.x - p1.x);
    float b = p1.y - m * p1.x;
    float y = m * x + b;
    return y;
}

float getXForYOnLine(Position p1, Position p2, float y){
    if(p2.y < p1.y){
        swapValues(p2, p1, Position);
    }
    if(y < p1.y || y > p2.y){
        assert("getYForyOnLine: y passed must be in the range p1.y to p2.y" && 0);
    }
    if(p1.y == p2.y){
        //printf("p1.y == p2.y");
        return p1.x;
    }
    float m = (p2.x - p1.x)/(p2.y - p1.y);
    float b = p1.x - m * p1.y;
    float x = m * y + b;
    return x;
}

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
    if(end < start){
        swapValues(end, start, float);
    }

    if(val >= start && val <= end){
        return true;
    }

    return false;

}


void breakOnCondition(bool condition){
    if(condition == true){
        int x = 0;
    }
}