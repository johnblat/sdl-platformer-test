#include "v2d.h"
#include "util.h"
#include "ray2d.h"
#include "collisions.h"

bool collisions_Ray2d_intersects_line_segment(Ray2d ray, Position p1, Position p2, float &distanceFromRayOrigin, SensorType sensorType){
    if(sensorType == LF_SENSOR || sensorType == RF_SENSOR){
        if(!util_is_in_range(p1.x, p2.x, ray.startingPosition.x)){
            return false;
        }
        float y = util_get_y_for_x_on_line(p1, p2, ray.startingPosition.x);
        distanceFromRayOrigin = y - ray.startingPosition.y;
        if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
            return false;
        }
        return true;
    }
    else if(sensorType == RW_SENSOR){
        if(!util_is_in_range(p1.y, p2.y, ray.startingPosition.y)){
            return false;
        }
        float x = util_get_x_for_y_on_line(p1, p2, ray.startingPosition.y);
        distanceFromRayOrigin = x - ray.startingPosition.x;
        if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
            return false;
        }
        return true;
    }
    else if(sensorType == LW_SENSOR){
        if(!util_is_in_range(p1.y, p2.y, ray.startingPosition.y)){
            return false;
        }
        float x = util_get_x_for_y_on_line(p1, p2, ray.startingPosition.y);
        distanceFromRayOrigin = ray.startingPosition.x - x;
        if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
            return false;
        }
        return true;
    }

    return false;
}


bool collisions_point_intersects_point_with_tolerance(v2d p1, v2d p2, float tolerance){
    v2d v = p1 - p2;
    float distance = v2d_magnitude(v);
    if(fabs(distance) <= tolerance){
        return true;
    }
    return false;
}

bool collisions_horizontal_Ray2d_intersect_line_segment(Ray2d ray, v2d linePoint1, v2d linePoint2){
    if(!util_is_in_range(linePoint1.y, linePoint2.y, ray.startingPosition.y)){
        return false;
    }
    float x = util_get_x_for_y_on_line(linePoint1, linePoint2, ray.startingPosition.y);
    float distanceFromRayStartPoint = fabs(x - ray.startingPosition.x);
    if(distanceFromRayStartPoint > ray.distance || distanceFromRayStartPoint < 0){
        return false;
    }
    return true;
}


bool collisions_vertical_Ray2d_intersect_line_segment(Ray2d ray, v2d linePoint1, v2d linePoint2){
    if(!util_is_in_range(linePoint1.x, linePoint2.x, ray.startingPosition.x)){
        return false;
    }
    float y = util_get_y_for_x_on_line(linePoint1, linePoint2, ray.startingPosition.x);
    float distanceFromRayStartPoint = fabs(y - ray.startingPosition.y);
    if(distanceFromRayStartPoint > ray.distance || distanceFromRayStartPoint < 0){
        return false;
    }
    return true;
}

// Maybe instead of tolerance, I can give the line a "thickness"
bool collisions_point_intersect_line_segment_with_tolerance(v2d linePoint1, v2d linePoint2, v2d p, float tolerance ){
    Ray2d ray;
    ray.startingPosition = p;
    ray.distance = tolerance;

    if(collisions_vertical_Ray2d_intersect_line_segment(ray, linePoint1, linePoint2)){
        return true;
    }
    else if(collisions_horizontal_Ray2d_intersect_line_segment(ray, linePoint1, linePoint2)){
        return true;
    }
    return false;
}