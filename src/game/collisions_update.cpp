#include "collisions.h"
#include "states.h"
#include "position.h"
#include "flecs.h"
#include "velocity.h"
#include <vector>
#include "ray2d.h"
#include "solid_rect.h"
#include "ints.h"
#include "v2d.h"
#include "util.h"
#include <cassert>
#include "state_util.h"



void collisions_Sensors_wall_sensors_set_distance_from_velocity_System(flecs::iter &it, Sensors *sensorCollections, Velocity *velocities, GroundMode *groundModes){
    const float SENSORS_DEFAULT_WALL_DISTANCE = 8.0f;

    for(u64 i : it){
        sensorCollections[i].rays[SENSOR_LEFT_WALL].distance = 8.0f;
        sensorCollections[i].rays[SENSOR_RIGHT_WALL].distance = 8.0f;

        if(groundModes[i] == GROUND_MODE_FLOOR || groundModes[i] == GROUND_MODE_CEILING){
            sensorCollections[i].rays[SENSOR_LEFT_WALL].distance += fabs(velocities[i].x);
            sensorCollections[i].rays[SENSOR_RIGHT_WALL].distance += fabs(velocities[i].x);
        }
        else if(groundModes[i] == GROUND_MODE_LEFT_WALL || groundModes[i] == GROUND_MODE_RIGHT_WALL){
            sensorCollections[i].rays[SENSOR_LEFT_WALL].distance += fabs(velocities[i].y);
            sensorCollections[i].rays[SENSOR_RIGHT_WALL].distance += fabs(velocities[i].y);
        }
    }
}


void collisions_Sensors_wall_update_Position_System(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles){
    const float SENSORS_DEFAULT_WALL_DISTANCE = 8.0f;

    for(u64 i : it){
        Ray2d ray2d_left_wall_local, ray2d_left_wall_world; 
        ray2d_left_wall_local = sensorCollections[i].rays[SENSOR_LEFT_WALL];
        ray2d_left_wall_world.position_start = util_v2d_local_to_world(ray2d_left_wall_local.position_start, positions[i]);
        ray2d_left_wall_world.distance = ray2d_left_wall_local.distance;

        Ray2d ray2d_right_wall_local, ray2d_right_wall_world; 
        ray2d_right_wall_local = sensorCollections[i].rays[SENSOR_RIGHT_WALL];
        ray2d_right_wall_world.position_start = util_v2d_local_to_world(ray2d_right_wall_local.position_start, positions[i]);
        ray2d_right_wall_world.distance = ray2d_right_wall_local.distance;

        auto f = it.world().filter<Position, PlatformPath>();

        f.each([&](flecs::entity e, Position &platformPath_position, PlatformPath &platformPath){
                //walls
            for(int j = 0; j < platformPath.nodes.size() - 1; j++){
                Position local_position_node_current = platformPath.nodes.at(j);
                Position local_position_node_next = platformPath.nodes.at(j+1);

                v2d world_position_node_current = util_v2d_local_to_world(local_position_node_current, platformPath_position);
                v2d world_position_node_next = util_v2d_local_to_world(local_position_node_next, platformPath_position);

                v2d world_position_node_current_rotated = world_position_node_current;
                v2d world_position_node_next_rotated = world_position_node_next;

                if(groundModes[i] == GROUND_MODE_RIGHT_WALL && states[i].currentState == STATE_ON_GROUND){
                    world_position_node_current_rotated = v2d_rotate_90_degrees_clockwise(world_position_node_current, positions[i]);
                    world_position_node_next_rotated = v2d_rotate_90_degrees_clockwise(world_position_node_next, positions[i]);
                }
                else if(groundModes[i] == GROUND_MODE_LEFT_WALL && states[i].currentState == STATE_ON_GROUND){
                    world_position_node_current_rotated = v2d_rotate_90_degrees_counter_clockwise(world_position_node_current, positions[i]);
                    world_position_node_next_rotated = v2d_rotate_90_degrees_counter_clockwise(world_position_node_next, positions[i]);
                }
                else if(groundModes[i] == GROUND_MODE_CEILING && states[i].currentState == STATE_ON_GROUND){
                    world_position_node_current_rotated = v2d_rotate_180_degrees(world_position_node_current, positions[i]);
                    world_position_node_next_rotated = v2d_rotate_180_degrees(world_position_node_next, positions[i]);
                }

                float intersection_distance_from_ray_origin;
                v2d p_intersection;
                
                // is wall?
                if(true){
                    // moving right
                    if(velocities[i].x > 0){
                       if(collisions_Ray2d_intersects_line_segment(ray2d_right_wall_world, SENSOR_RIGHT_WALL, world_position_node_current_rotated, world_position_node_next_rotated, intersection_distance_from_ray_origin, p_intersection)){
                           positions[i].x = world_position_node_current.x - SENSORS_DEFAULT_WALL_DISTANCE;
                            velocities[i].x = 0;
                            groundSpeeds[i].val = 0.0f;
                            velocities[i].x = 0.0f;
                        }
                    }
                    // moving left
                    else if(velocities[i].x < 0) {
                        if(collisions_Ray2d_intersects_line_segment(ray2d_left_wall_world, SENSOR_LEFT_WALL,world_position_node_current_rotated, world_position_node_next_rotated, intersection_distance_from_ray_origin,p_intersection )){
                            positions[i].x = world_position_node_current.x + SENSORS_DEFAULT_WALL_DISTANCE;
                            velocities[i].x = 0;
                            groundSpeeds[i].val = 0.0f;
                            velocities[i].x = 0.0f;
                        }
                    }
                }
                
            }
        });
    }

}

void collisions_Sensors_wall_set_height_from_Angle_System(flecs::iter &it, Sensors *sensorCollections, Angle *angles){
    for(u64 i : it){

        const float FLAT_ENOUGH_ANGLE = 5.0f;
        const float FLAT_ENOUGH_WALL_SENSOR_HEIGHT = 8.0f;
        const float NOT_FLAT_WALL_SENSOR_HEIGHT = 0.0f;

        if(util_rads_to_degrees(angles[i].rads) < FLAT_ENOUGH_ANGLE && util_rads_to_degrees(angles[i].rads) > -FLAT_ENOUGH_ANGLE){
            sensorCollections[i].rays[SENSOR_RIGHT_WALL].position_start.y = FLAT_ENOUGH_WALL_SENSOR_HEIGHT;
            sensorCollections[i].rays[SENSOR_LEFT_WALL].position_start.y = FLAT_ENOUGH_WALL_SENSOR_HEIGHT;
        }
        else {
            sensorCollections[i].rays[SENSOR_RIGHT_WALL].position_start.y = NOT_FLAT_WALL_SENSOR_HEIGHT;
            sensorCollections[i].rays[SENSOR_LEFT_WALL].position_start.y = NOT_FLAT_WALL_SENSOR_HEIGHT;
        }
    }
}

void collisions_GroundMode_update_from_Angle_System(flecs::iter &it, GroundMode *groundModes, Angle *angles){
    for(u64 i : it){
        groundModes[i] = util_rads_to_ground_mode(angles[i].rads);
    }
}



// void collisions_Angle_update_from_intersecting_sensors(flecs::iter &it, Angle)

bool v2d_equal(v2d v1, v2d v2){
    if(v1.x == v2.x && v1.y == v2.y){
        return true;
    }
    return false;
}

bool lines_equal(v2d line_start_1, v2d line_end_1, v2d line_start_2, v2d line_end_2){
    if(v2d_equal(line_start_1, line_start_2) && v2d_equal(line_end_1, line_end_2)){
        return true;
    }
    return false;
}


Ray2d ray2d_local_to_world(Position world_position, Ray2d ray2d_local){
    Ray2d ray2d_world;
    ray2d_world.distance = ray2d_local.distance;
    ray2d_world.position_start = v2d_add(world_position, ray2d_local.position_start);

    return ray2d_world;
}

void collisions_Sensors_PlatformPaths_update_Angle_System(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles ){
    
    for(u64 i : it){
        
        State state = STATE_IN_AIR;

        CollisionResultRay2dIntersectLine collision_result_closest_left_floor_sensor;
        collision_result_closest_left_floor_sensor.did_intersect = false;

        CollisionResultRay2dIntersectLine collision_result_closest_right_floor_sensor;
        collision_result_closest_right_floor_sensor.did_intersect = false;

        v2d point_closest_left_floor_sensor_intersection_rotated(
            FLT_MAX,
            FLT_MAX
        );

        v2d point_closest_intersection_right_floor_sensor_rotated(
            FLT_MAX,
            FLT_MAX
        );

        v2d p1_intersecting_line_left_floor_sensor_rotated;
        v2d p2_intersecting_line_left_floor_sensor_rotated;

        v2d p1_intersecting_line_right_floor_sensor_rotated;
        v2d p2_intersecting_line_right_floor_sensor_rotated;



        // ensure height is correct for in air collision check
        if(states[i].currentState == STATE_IN_AIR){
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

        }

        Ray2d left_floor_sensor_ray_world = ray2d_local_to_world(
            positions[i], 
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR]
        );


        Ray2d right_floor_sensor_ray_world = ray2d_local_to_world(
            positions[i], 
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR]
        );

        auto f = it.world().filter<Position, PlatformPath>();

        f.each([&](flecs::entity e, Position &world_position, PlatformPath &platformPath){

            for(int j = 0; j < platformPath.nodes.size() - 1; j++){                
                v2d p1_local_platform_line = platformPath.nodes.at(j);
                v2d p2_local_platform_line = platformPath.nodes.at(j+1);

                v2d p1_world_platform_line = util_v2d_local_to_world(
                    p1_local_platform_line, 
                    world_position 
                );
                v2d p2_world_platform_line = util_v2d_local_to_world(
                    p2_local_platform_line, 
                    world_position 
                );

                v2d p1_world_rotated_platform_line = v2d_rotate(
                    p1_world_platform_line, 
                    positions[i], 
                    angles[i].rads
                );
                v2d p2_world_rotated_platform_line = v2d_rotate(
                    p2_world_platform_line, 
                    positions[i], 
                    angles[i].rads
                );
                
                

                CollisionResultRay2dIntersectLine collision_result_left_floor_sensor_rotated = collisions_Ray2d_intersects_line_segment_result(
                    left_floor_sensor_ray_world, 
                    SENSOR_LEFT_FLOOR,
                    p1_world_rotated_platform_line, 
                    p2_world_rotated_platform_line
                );

                if(collision_result_left_floor_sensor_rotated.did_intersect){
                    state = STATE_ON_GROUND;

                    if(collision_result_left_floor_sensor_rotated.p_world_intersection.y < point_closest_left_floor_sensor_intersection_rotated.y){
                        point_closest_left_floor_sensor_intersection_rotated = collision_result_left_floor_sensor_rotated.p_world_intersection;

                        p1_intersecting_line_left_floor_sensor_rotated = p1_world_rotated_platform_line;
                        p2_intersecting_line_left_floor_sensor_rotated = p2_world_rotated_platform_line;

                        collision_result_closest_left_floor_sensor = collision_result_closest_left_floor_sensor;
                        collision_result_closest_left_floor_sensor.p_world_intersection = v2d_rotate(collision_result_closest_left_floor_sensor.p_world_intersection, positions[i], -angles[i].rads);

                    }
                }

                CollisionResultRay2dIntersectLine collision_result_right_floor_sensor = collisions_Ray2d_intersects_line_segment_result(
                    right_floor_sensor_ray_world, 
                    SENSOR_RIGHT_FLOOR,
                    p1_world_rotated_platform_line, 
                    p2_world_rotated_platform_line
                );

                if(collision_result_right_floor_sensor.did_intersect){
                    state = STATE_ON_GROUND;

                    if(collision_result_right_floor_sensor.p_world_intersection.y < point_closest_intersection_right_floor_sensor_rotated.y){
                        point_closest_intersection_right_floor_sensor_rotated = collision_result_right_floor_sensor.p_world_intersection;

                        collision_result_closest_right_floor_sensor.p1_intersecting_line = v2d_rotate(p1_world_rotated_platform_line, positions[i], -angles[i].rads);
                        collision_result_closest_right_floor_sensor.p2_intersecting_line = v2d_rotate(p2_world_rotated_platform_line, positions[i], -angles[i].rads);

                        collision_result_closest_right_floor_sensor = collision_result_closest_right_floor_sensor;

                        collision_result_closest_right_floor_sensor.p_world_intersection = v2d_rotate(collision_result_closest_right_floor_sensor.p_world_intersection,  positions[i], -angles[i].rads);
                    }
                }
    
            } 
            
        });

        State_util_set(states[i], state);


        // they were rotated around player when being checked for collision, so now rotate them back to their original positions
        v2d p1_intersecting_line_left_floor_sensor = v2d_rotate(p1_intersecting_line_left_floor_sensor_rotated, positions[i], -angles[i].rads);
        v2d p2_intersecting_line_left_floor_sensor = v2d_rotate(p2_intersecting_line_left_floor_sensor_rotated, positions[i], -angles[i].rads);

        v2d p1_intersecting_line_right_floor_sensor = v2d_rotate(p1_intersecting_line_right_floor_sensor_rotated, positions[i], -angles[i].rads);
        v2d p2_intersecting_line_right_floor_sensor = v2d_rotate(p2_intersecting_line_right_floor_sensor_rotated, positions[i], -angles[i].rads);

        // the direction of vector the player will be standing on
        // used to determine angle
        v2d v_direction;

        if(collision_result_closest_left_floor_sensor.did_intersect && !collision_result_closest_right_floor_sensor.did_intersect){

            v_direction = v2d_sub(
                p1_intersecting_line_left_floor_sensor,
                p2_intersecting_line_left_floor_sensor
            );

        }
        else if(!collision_result_closest_left_floor_sensor.did_intersect && collision_result_closest_right_floor_sensor.did_intersect){

            v_direction = v2d_sub(
                p1_intersecting_line_right_floor_sensor,
                p2_intersecting_line_right_floor_sensor
            );

        }
        else if(collision_result_closest_left_floor_sensor.did_intersect && collision_result_closest_right_floor_sensor.did_intersect){
            bool closest_intersections_intersecting_same_line = lines_equal(
                p1_intersecting_line_left_floor_sensor,
                p2_intersecting_line_left_floor_sensor,
                p1_intersecting_line_right_floor_sensor,
                p2_intersecting_line_right_floor_sensor
            );
            
            if(closest_intersections_intersecting_same_line){
                v_direction = v2d_sub(
                    p1_intersecting_line_left_floor_sensor_rotated,
                    p2_intersecting_line_left_floor_sensor_rotated
                );
            }
            else{ // since they are different lines, try to find the "difference" between them to get an angle "between" the two  angles
                
                v2d v_direction_closest_intersecting_line_left_floor_sensor = v2d_sub(
                    p1_intersecting_line_left_floor_sensor,
                    p2_intersecting_line_left_floor_sensor
                );

                v2d v_direction_closest_intersecting_line_right_floor_sensor = v2d_sub(
                    p2_intersecting_line_right_floor_sensor,
                    p1_intersecting_line_right_floor_sensor
                );

                v2d v_unit_direction_closest_intersecting_line_left_floor_sensor = v2d_unit(
                    v_direction_closest_intersecting_line_left_floor_sensor
                );

                v2d v_unit_direction_closest_intersecting_line_right_floor_sensor = v2d_unit(
                    v_direction_closest_intersecting_line_right_floor_sensor
                );

                v2d v_direction_closest_intersecting_line_right_to_left = v2d_sub(
                    v_unit_direction_closest_intersecting_line_right_floor_sensor,
                    v_unit_direction_closest_intersecting_line_left_floor_sensor
                );

                v2d v_direction_closest_intersecting_line_right_to_left_unit = v2d_unit(
                    v_direction_closest_intersecting_line_right_to_left
                );

                v_direction = v_direction_closest_intersecting_line_right_to_left_unit;
            }

            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;

            if(groundModes[i] == GROUND_MODE_FLOOR){
                if(v_direction.x < 0){
                    v_direction.x *= -1;
                    v_direction.y *= -1;
                }
            }
            else if(groundModes[i] == GROUND_MODE_CEILING){
                if(v_direction.x > 0){
                    v_direction.x *= -1;
                    v_direction.y *= -1;
                }
            }
            else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
                if(v_direction.y < 0){
                    v_direction.x *= -1;
                    v_direction.y *= -1;
                }
            }
            else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
                if(v_direction.y > 0){
                    v_direction.x *= -1;
                    v_direction.y *= -1;
                }
            }

            angles[i].rads = atan2(-v_direction.y, v_direction.x);
            if(angles[i].rads < 0){
                const float maxRads = 3.14 * 2;
                angles[i].rads = maxRads +angles[i].rads;
            }

        }
        else{
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            groundModes[i] = GROUND_MODE_FLOOR;
            angles[i].rads = 0.0f;
        }
        groundModes[i] = util_rads_to_ground_mode(angles[i].rads);
    }
}

void collisions_Sensors_PlatformPaths_update_Position_System(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles ){
    
    for(u64 i : it){
        
        State state = STATE_IN_AIR;

        v2d point_closest_intersection_left_floor_sensor(
            FLT_MAX,
            FLT_MAX
        );

        v2d point_closest_intersection_right_floor_sensor(
            FLT_MAX,
            FLT_MAX
        );

        v2d point_closest_intersection_point(
            FLT_MAX,
            FLT_MAX
        );

        v2d p1_closest_intersecting_line_left_floor_sensor;
        v2d p2_closest_intersecting_line_left_floor_sensor;

        v2d p1_closest_intersecting_line_right_floor_sensor;
        v2d p2_closest_intersecting_line_right_floor_sensor;

        v2d p1_closest_intersecting_line;
        v2d p2_closest_intersecting_line;

        // is in air
        if(states[i].currentState == STATE_IN_AIR){
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

        }

        Ray2d left_floor_sensor_ray_local = sensorCollections[i].rays[SENSOR_LEFT_FLOOR];
        Ray2d left_floor_sensor_ray_world;
        left_floor_sensor_ray_world.position_start.x = positions[i].x + left_floor_sensor_ray_local.position_start.x;
        left_floor_sensor_ray_world.position_start.y = positions[i].y + left_floor_sensor_ray_local.position_start.y;
        left_floor_sensor_ray_world.distance = left_floor_sensor_ray_local.distance;

        Ray2d right_floor_sensor_ray_local = sensorCollections[i].rays[SENSOR_RIGHT_FLOOR];
        Ray2d right_floor_sensor_ray_world;
        right_floor_sensor_ray_world.position_start.x = positions[i].x + right_floor_sensor_ray_local.position_start.x;
        right_floor_sensor_ray_world.position_start.y = positions[i].y + right_floor_sensor_ray_local.position_start.y;
        right_floor_sensor_ray_world.distance = right_floor_sensor_ray_local.distance;

        float closest_distance_from_point = FLT_MAX;

        auto f = it.world().filter<Position, PlatformPath>();

        f.each([&](flecs::entity e, Position &position, PlatformPath &platformPath){
            size_t len = platformPath.nodes.size();
            for(int v = 0; v < len - 1; v++){                
                v2d p1 = platformPath.nodes.at(v);
                v2d p2 = platformPath.nodes.at(v+1);

                v2d v1 = v2d_add(p1, position);
                v2d v2 = v2d_add(p2, position);

                v2d r1 = v2d_rotate(v1, positions[i], angles[i].rads);
                v2d r2 = v2d_rotate(v2, positions[i], angles[i].rads);
                
                float distance_from_left_floor_sensor_origin;
                v2d 

                if(collisions_Ray2d_intersects_line_segment(left_floor_sensor_ray_world, r1, r2, distance_from_left_floor_sensor_origin, SENSOR_LEFT_FLOOR)){

                    state = STATE_ON_GROUND;

                    v2d intersectionPoint(
                            left_floor_sensor_ray_world.position_start.x,
                            left_floor_sensor_ray_world.position_start.y + distance_from_left_floor_sensor_origin
                    );

                    if(intersectionPoint.y < point_closest_intersection_left_floor_sensor.y){
                        point_closest_intersection_left_floor_sensor = intersectionPoint;
                        p1_closest_intersecting_line_left_floor_sensor = r1;
                        p2_closest_intersecting_line_left_floor_sensor = r2;
                        closest_distance_from_point = distance_from_left_floor_sensor_origin;
                    }
                
                }

                float distance_from_right_floor_sensor_origin;

                if(collisions_Ray2d_intersects_line_segment(right_floor_sensor_ray_world, r1, r2, distance_from_right_floor_sensor_origin, SENSOR_RIGHT_FLOOR)){

                    state = STATE_ON_GROUND;

                    v2d intersectionPoint(
                            right_floor_sensor_ray_world.position_start.x,
                            right_floor_sensor_ray_world.position_start.y + distance_from_right_floor_sensor_origin
                    );

                    if(intersectionPoint.y < point_closest_intersection_right_floor_sensor.y){
                        point_closest_intersection_right_floor_sensor = intersectionPoint;
                        p1_closest_intersecting_line_right_floor_sensor = r1;
                        p2_closest_intersecting_line_right_floor_sensor = r2;
                        closest_distance_from_point = distance_from_right_floor_sensor_origin;

                    }
                }

                
                
            }  
        });

        if(point_closest_intersection_left_floor_sensor.y < point_closest_intersection_right_floor_sensor.y){
            point_closest_intersection_point = point_closest_intersection_left_floor_sensor;
            p1_closest_intersecting_line = p1_closest_intersecting_line_left_floor_sensor;
            p2_closest_intersecting_line = p2_closest_intersecting_line_left_floor_sensor;
        }
        else{
            point_closest_intersection_point = point_closest_intersection_right_floor_sensor;
            p1_closest_intersecting_line = p1_closest_intersecting_line_right_floor_sensor;
            p2_closest_intersecting_line = p2_closest_intersecting_line_right_floor_sensor;
        }

        // rotate highest points back to their normal state
        point_closest_intersection_point = v2d_rotate(point_closest_intersection_point, positions[i], -angles[i].rads);
        p1_closest_intersecting_line = v2d_rotate(p1_closest_intersecting_line, positions[i], -angles[i].rads);
        p2_closest_intersecting_line = v2d_rotate(p2_closest_intersecting_line, positions[i], -angles[i].rads);
        

        if(groundModes[i] == GROUND_MODE_FLOOR){
                if(p2_closest_intersecting_line.x < p1_closest_intersecting_line.x){
                    swapValues(p2_closest_intersecting_line, p1_closest_intersecting_line, Position);
                }
            }
            else if(groundModes[i] == GROUND_MODE_CEILING){
                if(p2_closest_intersecting_line.x > p1_closest_intersecting_line.x){
                    swapValues(p2_closest_intersecting_line, p1_closest_intersecting_line, Position);
                }
            }
            else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
                if(p2_closest_intersecting_line.y < p1_closest_intersecting_line.y){
                    swapValues(p2_closest_intersecting_line, p1_closest_intersecting_line, Position);
                }
            }
            else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
                if(p2_closest_intersecting_line.y > p1_closest_intersecting_line.y){
                    swapValues(p2_closest_intersecting_line, p1_closest_intersecting_line, Position);
                }
            }

        State_util_set(states[i], state);
        if(state == STATE_ON_GROUND){
            // get direction vector of line
            // Find perpendicular/normal of itnersecting line
            // nomralize/make unit vector
            // scale vector by closest distance
            // move player that amount
            v2d v_direction_closest_intersecting_line = v2d_sub(
                p2_closest_intersecting_line, 
                p1_closest_intersecting_line
            );
            
            v2d v_unit_closest_intersecting_line = v2d_unit(
                v_direction_closest_intersecting_line
            );
            
            v2d v_normal_closest_intersecting_line = v2d_perp(
                v_unit_closest_intersecting_line
            );

            v2d v_move_player = v2d_scale(
                HALF_PLAYER_HEIGHT - closest_distance_from_point,
                v_normal_closest_intersecting_line
            );

            positions[i] = v2d_add(positions[i], v_move_player);

            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            // if(groundModes[i] == GROUND_MODE_FLOOR){
            //     positions[i].y -= (HALF_PLAYER_HEIGHT - closest_distance_from_point);
                
            // }
            // else if(groundModes[i] == GROUND_MODE_CEILING){
            //     positions[i].y +=  (HALF_PLAYER_HEIGHT - closest_distance_from_point);
                
            // }
            // else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
            //     positions[i].x += (HALF_PLAYER_HEIGHT - closest_distance_from_point);
                
            // }
            // else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
            //     positions[i].x -= (HALF_PLAYER_HEIGHT - closest_distance_from_point);
               
            // }
            
            
        }
        else {
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            groundModes[i] = GROUND_MODE_FLOOR;
            angles[i].rads = 0.0f;
        }
    }
}


