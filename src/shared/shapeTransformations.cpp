//
// Created by frog on 2/25/22.
//
#include "shapes.h"
#include "position.h"
#include "solid_rect.h"
#include "shapeTransformations.h"

RectVertices generateRotatedRectVertices(SolidRect rect, Position center){
    RectVertices initialVertices(
            v2d(center.x - rect.w/2, center.y - rect.h/2),
            v2d(center.x + rect.w/2, center.y - rect.h/2),
            v2d(center.x + rect.w/2, center.y + rect.h/2),
            v2d(center.x - rect.w/2, center.y + rect.h/2)
    );
    RectVertices rotatedVertices;
    for(int i = 0; i < 4; i++){
        rotatedVertices[i] = v2d_rotate(initialVertices[i], v2d(center.x, center.y), rect.rotation);
    }
    return rotatedVertices;
}
