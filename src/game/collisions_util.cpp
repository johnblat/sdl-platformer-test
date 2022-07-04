#include "circle.h"
#include "collisions.h"
#include "ray2d.h"
#include "util.h"
#include "v2d.h"


bool collisions_Ray2d_intersects_line_segment(Ray2d ray, Position p1, Position p2, float &distanceFromRayOrigin, SensorType sensorType){
    if(sensorType == SENSOR_LEFT_FLOOR || sensorType == SENSOR_RIGHT_FLOOR || sensorType == SENSOR_CENTER_FLOOR){
        if(!util_is_in_range(p1.x, p2.x, ray.position_start.x)){
            return false;
        }
        float y = util_get_y_for_x_on_line(p1, p2, ray.position_start.x);
        distanceFromRayOrigin = y - ray.position_start.y;
        if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
            return false;
        }
        return true;
    }
    else if(sensorType == SENSOR_RIGHT_WALL){
        if(!util_is_in_range(p1.y, p2.y, ray.position_start.y)){
            return false;
        }
        float x = util_get_x_for_y_on_line(p1, p2, ray.position_start.y);
        distanceFromRayOrigin = x - ray.position_start.x;
        if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
            return false;
        }
        return true;
    }
    else if(sensorType == SENSOR_LEFT_WALL){
        util_break_on_condition(p1.x == p2.x);
        if(!util_is_in_range(p1.y, p2.y, ray.position_start.y)){
            return false;
        }
        float x = util_get_x_for_y_on_line(p1, p2, ray.position_start.y);
        distanceFromRayOrigin = ray.position_start.x - x;
        if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
            return false;
        }
        return true;
    }

    return false;
}

CollisionResultRay2dIntersectLine collisions_Ray2d_intersects_line_segment_result(Ray2d ray_sensor, SensorType sensor_type, Position p1_line, Position p2_line){
    CollisionResultRay2dIntersectLine collision_result;

    if(sensor_type == SENSOR_LEFT_FLOOR || sensor_type == SENSOR_RIGHT_FLOOR || sensor_type == SENSOR_CENTER_FLOOR){

        if(!util_is_in_range(p1_line.x, p2_line.x, ray_sensor.position_start.x)){
            collision_result.did_intersect = false;
            return collision_result;
        }

        float y = util_get_y_for_x_on_line(p1_line, p2_line, ray_sensor.position_start.x);

        collision_result.distance_from_ray_origin = y - ray_sensor.position_start.y;

        collision_result.p_world_intersection = v2d(
            ray_sensor.position_start.x,
            ray_sensor.position_start.y + collision_result.distance_from_ray_origin
        );

        if(collision_result.distance_from_ray_origin > ray_sensor.distance || collision_result.distance_from_ray_origin < 0){
            collision_result.did_intersect = false;
            return collision_result;
        }

        collision_result.did_intersect = true;

        return collision_result;
    }
    else if(sensor_type == SENSOR_RIGHT_WALL){
        
        if(!util_is_in_range(p1_line.y, p2_line.y, ray_sensor.position_start.y)){
            collision_result.did_intersect = false;
            return collision_result;
        }
        
        float x = util_get_x_for_y_on_line(p1_line, p2_line, ray_sensor.position_start.y);
        
        collision_result.distance_from_ray_origin = x - ray_sensor.position_start.x;
        
        collision_result.p_world_intersection = v2d(
            ray_sensor.position_start.x,
            ray_sensor.position_start.y + collision_result.distance_from_ray_origin
        );

        if(collision_result.distance_from_ray_origin > ray_sensor.distance || collision_result.distance_from_ray_origin < 0){
            collision_result.did_intersect = false;
            return collision_result;
        }

        collision_result.did_intersect = true;

        return collision_result;
    }
    else if(sensor_type == SENSOR_LEFT_WALL){
        util_break_on_condition(p1_line.x == p2_line.x);
        if(!util_is_in_range(p1_line.y, p2_line.y, ray_sensor.position_start.y)){
            collision_result.did_intersect = false;
            return collision_result;
        }

        float x = util_get_x_for_y_on_line(p1_line, p2_line, ray_sensor.position_start.y);

        collision_result.p_world_intersection = v2d(
            ray_sensor.position_start.x,
            ray_sensor.position_start.y + collision_result.distance_from_ray_origin
        );

        collision_result.distance_from_ray_origin = ray_sensor.position_start.x - x;

        if(collision_result.distance_from_ray_origin > ray_sensor.distance || collision_result.distance_from_ray_origin < 0){
            collision_result.did_intersect = false;
            return collision_result;
        }

        collision_result.did_intersect = true;

        return collision_result;
    }

    collision_result.did_intersect = false;

    return collision_result;
}



CollisionResultRay2dIntersectLine collisions_Ray2d_rotated_intersects_line_segment_result(Ray2d ray_sensor_world, SensorType sensor_type, float sensor_rotation_in_rads, v2d p_origin_of_rotation, Position p1_line, Position p2_line){
    CollisionResultRay2dIntersectLine collision_result;
    
    // The rays themselves do not rotate.
    // This is to make math easier.
    // Instead, the line being checked is rotated in the opposite direction around the rotation origin point.
    // This way, intersection calculation is easier because its just a straight line ray against an already rotated line.

    v2d p1_line_rotated = v2d_rotate(p1_line, p_origin_of_rotation, sensor_rotation_in_rads);
    v2d p2_line_rotated = v2d_rotate(p2_line, p_origin_of_rotation, sensor_rotation_in_rads);

    if(sensor_type == SENSOR_LEFT_FLOOR || sensor_type == SENSOR_RIGHT_FLOOR || sensor_type == SENSOR_CENTER_FLOOR){

        if(!util_is_in_range(p1_line_rotated.x, p2_line_rotated.x, ray_sensor_world.position_start.x)){
            collision_result.did_intersect = false;
            return collision_result;
        }

        float y = util_get_y_for_x_on_line(p1_line_rotated, p2_line_rotated, ray_sensor_world.position_start.x);

        collision_result.distance_from_ray_origin = y - ray_sensor_world.position_start.y;

        if(collision_result.distance_from_ray_origin > ray_sensor_world.distance || collision_result.distance_from_ray_origin < 0){
            collision_result.did_intersect = false;
            return collision_result;
        }

        v2d p_intersection_rotated = v2d(
            ray_sensor_world.position_start.x,
            ray_sensor_world.position_start.y + collision_result.distance_from_ray_origin
        );

        collision_result.p_world_intersection = v2d_rotate(p_intersection_rotated, p_origin_of_rotation, -sensor_rotation_in_rads);
        
        collision_result.p1_intersecting_line = p1_line;
        collision_result.p2_intersecting_line = p2_line;

        collision_result.did_intersect = true;

        return collision_result;
    }

    assert("collisions_Ray2d_rotated_intersects_line_segment_result() received non floor sensor. Has not been accounted for yet.");
    // else if(sensor_type == SENSOR_RIGHT_WALL){
        
    //     if(!util_is_in_range(p1_line.y, p2_line.y, ray_sensor_world.position_start.y)){
    //         collision_result.did_intersect = false;
    //         return collision_result;
    //     }
        
    //     float x = util_get_x_for_y_on_line(p1_line, p2_line, ray_sensor_world.position_start.y);
        
    //     collision_result.distance_from_ray_origin = x - ray_sensor_world.position_start.x;
        
    //     collision_result.p_world_intersection = v2d(
    //         ray_sensor_world.position_start.x,
    //         ray_sensor_world.position_start.y + collision_result.distance_from_ray_origin
    //     );

    //     if(collision_result.distance_from_ray_origin > ray_sensor_world.distance || collision_result.distance_from_ray_origin < 0){
    //         collision_result.did_intersect = false;
    //         return collision_result;
    //     }

    //     collision_result.did_intersect = true;

    //     return collision_result;
    // }
    // else if(sensor_type == SENSOR_LEFT_WALL){
    //     util_break_on_condition(p1_line.x == p2_line.x);
    //     if(!util_is_in_range(p1_line.y, p2_line.y, ray_sensor_world.position_start.y)){
    //         collision_result.did_intersect = false;
    //         return collision_result;
    //     }

    //     float x = util_get_x_for_y_on_line(p1_line, p2_line, ray_sensor_world.position_start.y);

    //     collision_result.p_world_intersection = v2d(
    //         ray_sensor_world.position_start.x,
    //         ray_sensor_world.position_start.y + collision_result.distance_from_ray_origin
    //     );

    //     collision_result.distance_from_ray_origin = ray_sensor_world.position_start.x - x;

    //     if(collision_result.distance_from_ray_origin > ray_sensor_world.distance || collision_result.distance_from_ray_origin < 0){
    //         collision_result.did_intersect = false;
    //         return collision_result;
    //     }

    //     collision_result.did_intersect = true;

    //     return collision_result;
    // }

    collision_result.did_intersect = false;

    return collision_result;
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


