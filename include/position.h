#ifndef posh
#define posh
#include "v2d.h"

typedef v2d Position;

struct Angle {
    float rads;
};

#define degToRads(deg)(deg * (3.14/180))

#endif