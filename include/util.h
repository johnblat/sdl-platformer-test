#pragma once
#include "velocity.h"

#define swapValues(a, b, T){ \
    T temp = a; \
    a = b; \
    b = temp; \
}


#define MAX(a, b)(a > b ? a : b)
#define MIN(a, b)(a < b ? a : b)
#define sign(x)(x >= 0 ? 1 : -1)


float rads2deg(float rads);
GroundMode whichGroundMode(float angleInRads);
bool inRange(float val, float start, float end);
