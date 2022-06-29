#include "circle.h"
#include "collisions.h"
#include "ray2d.h"
#include "util.h"
#include "v2d.h"


bool collisions_Ray2d_intersects_line_segment(Ray2d ray, Position p1, Position p2, float &distanceFromRayOrigin, SensorType sensorType){
    if(sensorType == SENSOR_LEFT_FLOOR || sensorType == SENSOR_RIGHT_FLOOR){
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
    else if(sensorType == SENSOR_RIGHT_WALL){
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
    else if(sensorType == SENSOR_LEFT_WALL){
        util_break_on_condition(p1.x == p2.x);
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


bool collisions_point_intersect_circle(v2d p, Circle circle){
    v2d v_circle_midpoint_to_p = v2d_sub(p, circle.midpoint);
    float magnitude_circle_midpoint_to_p = v2d_magnitude(v_circle_midpoint_to_p);
    if(magnitude_circle_midpoint_to_p <= circle.radius){
        return true;
    }
    return false;
}


bool collisions_line_segment_intersect_circle(v2d p1, v2d p2, Circle circle){
    if(collisions_point_intersect_circle(p1, circle)){
        return true;
    }
    if(collisions_point_intersect_circle(p2, circle)){
        return true;
    }

    float shortest_distance_from_circle_midpoint_to_line_segment =
        v2d_shortest_distance_from_point_to_line_segment(circle.midpoint, p1, p2);
    
    if(shortest_distance_from_circle_midpoint_to_line_segment <= circle.radius){
        return true;
    }
    
    return false;
}


