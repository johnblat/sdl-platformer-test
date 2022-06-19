
#include "ray2d.h"
#include "v2d.h"
#include "util.h"


bool PointIntersectPointWithTolerance(v2d p1, v2d p2, float tolerance){
    v2d v = p1 - p2;
    float distance = v2d_magnitude(v);
    if(fabs(distance) <= tolerance){
        return true;
    }
    return false;
}

bool HorizontalRayIntersectLineSegment(Ray2d ray, v2d linePoint1, v2d linePoint2){
    if(!isInRange(linePoint1.y, linePoint2.y, ray.startingPosition.y)){
        return false;
    }
    float x = getXForYOnLine(linePoint1, linePoint2, ray.startingPosition.y);
    float distanceFromRayStartPoint = fabs(x - ray.startingPosition.x);
    if(distanceFromRayStartPoint > ray.distance || distanceFromRayStartPoint < 0){
        return false;
    }
    return true;
}


bool VerticalRayIntersectLineSegment(Ray2d ray, v2d linePoint1, v2d linePoint2){
    if(!isInRange(linePoint1.x, linePoint2.x, ray.startingPosition.x)){
        return false;
    }
    float y = getYForXOnLine(linePoint1, linePoint2, ray.startingPosition.x);
    float distanceFromRayStartPoint = fabs(y - ray.startingPosition.y);
    if(distanceFromRayStartPoint > ray.distance || distanceFromRayStartPoint < 0){
        return false;
    }
    return true;
}

// Maybe instead of tolerance, I can give the line a "thickness"
bool PointIntersectLineWithTolerance(v2d linePoint1, v2d linePoint2, v2d p, float tolerance ){
    Ray2d ray;
    ray.startingPosition = p;
    ray.distance = tolerance;

    if(VerticalRayIntersectLineSegment(ray, linePoint1, linePoint2)){
        return true;
    }
    else if(HorizontalRayIntersectLineSegment(ray, linePoint1, linePoint2)){
        return true;
    }
    return false;
}