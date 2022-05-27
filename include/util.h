#pragma once
#include "velocity.h"
#include "position.h"

#define swapValues(a, b, T){ \
    T temp = a; \
    a = b; \
    b = temp; \
}


#define MAX(a, b)(a > b ? a : b)
#define MIN(a, b)(a < b ? a : b)
#define sign(x)(x >= 0 ? 1 : -1)


bool isInRange(float begin, float end, float x);

float getYForXOnLine(Position p1, Position p2, float x);

float getXForYOnLine(Position p1, Position p2, float y);

float rads2deg(float rads);
GroundMode whichGroundMode(float angleInRads);
bool inRange(float val, float start, float end);
void breakOnCondition(bool condition);