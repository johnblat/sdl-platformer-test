#pragma once

#include "ray2d.h"
#include "v2d.h"


bool PointIntersectPointWithTolerance(v2d p1, v2d p2, float tolerance);

bool HorizontalRayIntersectLineSegment(Ray2d ray, v2d linePoint1, v2d linePoint2);

bool VerticalRayIntersectLineSegment(Ray2d ray, v2d linePoint1, v2d linePoint2);
// Maybe instead of tolerance, I can give the line a "thickness"
bool PointIntersectLineWithTolerance(v2d linePoint1, v2d linePoint2, v2d p, float tolerance );