#ifndef shapes_h
#define shapes_h

#undef  V2D_IMPLEMENTATION
#include "v2d.h"
#include "ints.h"

struct RectVertices{
    v2d v1, v2, v3, v4;

    RectVertices(v2d V1, v2d V2, v2d V3, v2d V4) : v1(V1), v2(V2), v3(V3), v4(V4) {}
    RectVertices() :
            v1(0, 0),
            v2(0, 0),
            v3(0, 0),
            v4(0, 0) {}

    v2d &operator[](const u32 i){
        return (&v1)[i];
    }

    const v2d &operator[](const u32 i) const {
        return (&v1)[i];
    }
};

#endif