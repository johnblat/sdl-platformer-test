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
                // if(true){
                //     // moving right
                //     if(velocities[i].x > 0){
                //        if(collisions_Ray2d_intersects_line_segment(ray2d_right_wall_world, SENSOR_RIGHT_WALL, world_position_node_current_rotated, world_position_node_next_rotated, intersection_distance_from_ray_origin, p_intersection)){
                //            positions[i].x = world_position_node_current.x - SENSORS_DEFAULT_WALL_DISTANCE;
                //             velocities[i].x = 0;
                //             groundSpeeds[i].val = 0.0f;
                //             velocities[i].x = 0.0f;
                //         }
                //     }
                //     // moving left
                //     else if(velocities[i].x < 0) {
                //         if(collisions_Ray2d_intersects_line_segment(ray2d_left_wall_world, SENSOR_LEFT_WALL,world_position_node_current_rotated, world_position_node_next_rotated, intersection_distance_from_ray_origin,p_intersection )){
                //             positions[i].x = world_position_node_current.x + SENSORS_DEFAULT_WALL_DISTANCE;
                //             velocities[i].x = 0;
                //             groundSpeeds[i].val = 0.0f;
                //             velocities[i].x = 0.0f;
                //         }
                //     }
                // }
                
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


void internal_collisions_ray2d_find_closest_floor_intersection_on_platform_paths(
    flecs::world &world, 
    Ray2d left_floor_sensor_ray_world, 
    Ray2d right_floor_sensor_ray_world, 
    float rotation_in_rads, 
    v2d p_rotation_origin, 
    CollisionResultRay2dIntersectLine &closest_collision_result_left_floor, 
    CollisionResultRay2dIntersectLine &closest_collision_result_right_floor
){
    closest_collision_result_left_floor.did_intersect = false;
    closest_collision_result_right_floor.did_intersect = false;

    auto f = world.filter<Position, PlatformPath>();

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

        
            CollisionResultRay2dIntersectLine current_collision_result_left_floor = collisions_Ray2d_rotated_intersects_line_segment_result(
                left_floor_sensor_ray_world,
                SENSOR_LEFT_FLOOR,
                rotation_in_rads,
                p_rotation_origin,
                p1_world_platform_line,
                p2_world_platform_line
            );
            

            if(current_collision_result_left_floor.did_intersect){

                if(!closest_collision_result_left_floor.did_intersect){ // no closest one yet
                    closest_collision_result_left_floor = current_collision_result_left_floor;
                }
                else if(closest_collision_result_left_floor.distance_from_ray_origin < closest_collision_result_left_floor.distance_from_ray_origin){
                    closest_collision_result_left_floor = current_collision_result_left_floor;
                }
            }

            CollisionResultRay2dIntersectLine current_collision_result_right_floor = collisions_Ray2d_rotated_intersects_line_segment_result(
                right_floor_sensor_ray_world,
                SENSOR_RIGHT_FLOOR,
                rotation_in_rads,
                p_rotation_origin,
                p1_world_platform_line,
                p2_world_platform_line
            );
            

            if(current_collision_result_right_floor.did_intersect){

                if(!closest_collision_result_right_floor.did_intersect){
                    closest_collision_result_right_floor = current_collision_result_right_floor;
                }
                else if(current_collision_result_right_floor.distance_from_ray_origin < closest_collision_result_right_floor.distance_from_ray_origin){
                    closest_collision_result_right_floor = current_collision_result_right_floor;
                }
            }
        } 
    });

}

void collisions_Sensors_PlatformPaths_update_Angle_System(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles ){
    
    for(u64 i : it){
        
        CollisionResultRay2dIntersectLine closest_collision_result_left_floor_sensor;
        
        CollisionResultRay2dIntersectLine closest_collision_result_right_floor_sensor;

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

        flecs::world world = it.world();

        internal_collisions_ray2d_find_closest_floor_intersection_on_platform_paths(
            world,
            left_floor_sensor_ray_world,
            right_floor_sensor_ray_world,
            angles[i].rads,
            positions[i],
            closest_collision_result_left_floor_sensor,
            closest_collision_result_right_floor_sensor
        );

        // ALIGN LINES TO FACE CORRECT WAY
        {
        if(groundModes[i] == GROUND_MODE_FLOOR){
            if(closest_collision_result_left_floor_sensor.p2_intersecting_line.x < closest_collision_result_left_floor_sensor.p1_intersecting_line.x){
                swapValues(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_left_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
            if(closest_collision_result_right_floor_sensor.p2_intersecting_line.x < closest_collision_result_right_floor_sensor.p1_intersecting_line.x){
                swapValues(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_right_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
        }
        else if(groundModes[i] == GROUND_MODE_CEILING){
            if(closest_collision_result_left_floor_sensor.p2_intersecting_line.x > closest_collision_result_left_floor_sensor.p1_intersecting_line.x){
                swapValues(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_left_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
            if(closest_collision_result_right_floor_sensor.p2_intersecting_line.x > closest_collision_result_right_floor_sensor.p1_intersecting_line.x){
                swapValues(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_right_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
        }
        else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
            if(closest_collision_result_left_floor_sensor.p2_intersecting_line.y < closest_collision_result_left_floor_sensor.p1_intersecting_line.y){
                swapValues(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_left_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
            if(closest_collision_result_right_floor_sensor.p2_intersecting_line.y < closest_collision_result_right_floor_sensor.p1_intersecting_line.y){
                swapValues(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_right_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
        }
        else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
            if(closest_collision_result_left_floor_sensor.p2_intersecting_line.y > closest_collision_result_left_floor_sensor.p1_intersecting_line.y){
                swapValues(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_left_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
            if(closest_collision_result_right_floor_sensor.p2_intersecting_line.y > closest_collision_result_right_floor_sensor.p1_intersecting_line.y){
                swapValues(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_right_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
        }
        }

        // the direction of vector the player will be standing on
        // used to determine angle
        v2d v_direction;

        if(closest_collision_result_left_floor_sensor.did_intersect || closest_collision_result_right_floor_sensor.did_intersect){

            // State_util_set(states[i], STATE_ON_GROUND);

            if(closest_collision_result_left_floor_sensor.did_intersect && !closest_collision_result_right_floor_sensor.did_intersect){

                v_direction = v2d_sub(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line,
                    closest_collision_result_left_floor_sensor.p1_intersecting_line
                );

            }

            else if(!closest_collision_result_left_floor_sensor.did_intersect && closest_collision_result_right_floor_sensor.did_intersect){

                v_direction = v2d_sub(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line,
                    closest_collision_result_right_floor_sensor.p1_intersecting_line
                );

            }

            else if(closest_collision_result_left_floor_sensor.did_intersect && closest_collision_result_right_floor_sensor.did_intersect){
                bool closest_intersections_intersecting_same_line = lines_equal(
                    closest_collision_result_left_floor_sensor.p1_intersecting_line,
                    closest_collision_result_left_floor_sensor.p2_intersecting_line,
                    closest_collision_result_right_floor_sensor.p1_intersecting_line,
                    closest_collision_result_right_floor_sensor.p2_intersecting_line
                );
                
                if(closest_intersections_intersecting_same_line){
                    v_direction = v2d_sub(
                        closest_collision_result_left_floor_sensor.p2_intersecting_line,
                        closest_collision_result_left_floor_sensor.p1_intersecting_line
                    );
                }
                else{ // since they are different lines, try to find the "difference" between them to get an angle "between" the two  angles
                    
                    v2d v_direction_closest_intersecting_line_left_floor_sensor = v2d_sub(
                        closest_collision_result_left_floor_sensor.p1_intersecting_line,
                        closest_collision_result_left_floor_sensor.p2_intersecting_line
                    );

                    v2d v_direction_closest_intersecting_line_right_floor_sensor = v2d_sub(
                        closest_collision_result_right_floor_sensor.p2_intersecting_line,
                        closest_collision_result_right_floor_sensor.p1_intersecting_line
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
            }

            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;

            // if(groundModes[i] == GROUND_MODE_FLOOR){
            //     if(v_direction.x < 0){
            //         v_direction.x *= -1;
            //         v_direction.y *= -1;
            //     }
            // }
            // else if(groundModes[i] == GROUND_MODE_CEILING){
            //     if(v_direction.x > 0){
            //         v_direction.x *= -1;
            //         v_direction.y *= -1;
            //     }
            // }
            // else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
            //     if(v_direction.y < 0){
            //         v_direction.x *= -1;
            //         v_direction.y *= -1;
            //     }
            // }
            // else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
            //     if(v_direction.y > 0){
            //         v_direction.x *= -1;
            //         v_direction.y *= -1;
            //     }
            // }

            angles[i].rads = atan2(-v_direction.y, v_direction.x);

            if(angles[i].rads < 0){
                const float maxRads = 3.14 * 2;
                angles[i].rads = maxRads +angles[i].rads;
            }

        }
        else{ // did not intersect either sensor

            // State_util_set(states[i], STATE_IN_AIR);

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
        
        CollisionResultRay2dIntersectLine closest_collision_result_left_floor_sensor;
        
        CollisionResultRay2dIntersectLine closest_collision_result_right_floor_sensor;

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

        flecs::world world = it.world();

        internal_collisions_ray2d_find_closest_floor_intersection_on_platform_paths(
            world,
            left_floor_sensor_ray_world,
            right_floor_sensor_ray_world,
            angles[i].rads,
            positions[i],
            closest_collision_result_left_floor_sensor,
            closest_collision_result_right_floor_sensor
        );

        // ALIGN INTERSECTING LINES WITH GROUND MODE
        {
        if(groundModes[i] == GROUND_MODE_FLOOR){
            if(closest_collision_result_left_floor_sensor.p2_intersecting_line.x < closest_collision_result_left_floor_sensor.p1_intersecting_line.x){
                swapValues(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_left_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
            if(closest_collision_result_right_floor_sensor.p2_intersecting_line.x < closest_collision_result_right_floor_sensor.p1_intersecting_line.x){
                swapValues(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_right_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
        }
        else if(groundModes[i] == GROUND_MODE_CEILING){
            if(closest_collision_result_left_floor_sensor.p2_intersecting_line.x > closest_collision_result_left_floor_sensor.p1_intersecting_line.x){
                swapValues(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_left_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
            if(closest_collision_result_right_floor_sensor.p2_intersecting_line.x > closest_collision_result_right_floor_sensor.p1_intersecting_line.x){
                swapValues(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_right_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
        }
        else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
            if(closest_collision_result_left_floor_sensor.p2_intersecting_line.y < closest_collision_result_left_floor_sensor.p1_intersecting_line.y){
                swapValues(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_left_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
            if(closest_collision_result_right_floor_sensor.p2_intersecting_line.y < closest_collision_result_right_floor_sensor.p1_intersecting_line.y){
                swapValues(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_right_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
        }
        else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
            if(closest_collision_result_left_floor_sensor.p2_intersecting_line.y > closest_collision_result_left_floor_sensor.p1_intersecting_line.y){
                swapValues(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_left_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
            if(closest_collision_result_right_floor_sensor.p2_intersecting_line.y > closest_collision_result_right_floor_sensor.p1_intersecting_line.y){
                swapValues(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line, 
                    closest_collision_result_right_floor_sensor.p1_intersecting_line, 
                    Position
                );
            }
        }
        }

        // HANDLE COLLISIONS IF ANY

        // the direction of vector the player will be standing on
        // used to determine angle
        v2d v_direction;
        v2d v_direction_unit;
        v2d v_move_player(0.0f, 0.0f);

        if(closest_collision_result_left_floor_sensor.did_intersect || closest_collision_result_right_floor_sensor.did_intersect){

            State_util_set(states[i], STATE_ON_GROUND);

            if(closest_collision_result_left_floor_sensor.did_intersect && !closest_collision_result_right_floor_sensor.did_intersect){

                v_direction = v2d_sub(
                    closest_collision_result_left_floor_sensor.p2_intersecting_line,
                    closest_collision_result_left_floor_sensor.p1_intersecting_line
                );

                v_direction_unit = v2d_unit(v_direction);

                v2d v_perp_direction_unit = v2d_perp(
                    v_direction_unit
                );

                v_move_player = v2d_scale(
                    HALF_PLAYER_HEIGHT - closest_collision_result_left_floor_sensor.distance_from_ray_origin,
                    v_perp_direction_unit
                );                

            }

            else if(!closest_collision_result_left_floor_sensor.did_intersect && closest_collision_result_right_floor_sensor.did_intersect){

                v_direction = v2d_sub(
                    closest_collision_result_right_floor_sensor.p2_intersecting_line,
                    closest_collision_result_right_floor_sensor.p1_intersecting_line
                );

                v_direction_unit = v2d_unit(v_direction);

                v2d v_perp_direction_unit = v2d_perp(
                    v_direction_unit
                );

                v_move_player = v2d_scale(
                    HALF_PLAYER_HEIGHT - closest_collision_result_right_floor_sensor.distance_from_ray_origin,
                    v_perp_direction_unit
                );

            }

            else if(closest_collision_result_left_floor_sensor.did_intersect && closest_collision_result_right_floor_sensor.did_intersect){
                bool closest_intersections_intersecting_same_line = lines_equal(
                    closest_collision_result_left_floor_sensor.p1_intersecting_line,
                    closest_collision_result_left_floor_sensor.p2_intersecting_line,
                    closest_collision_result_right_floor_sensor.p1_intersecting_line,
                    closest_collision_result_right_floor_sensor.p2_intersecting_line
                );
                
                if(closest_intersections_intersecting_same_line){
                    v_direction = v2d_sub(
                        closest_collision_result_left_floor_sensor.p2_intersecting_line,
                        closest_collision_result_left_floor_sensor.p1_intersecting_line
                    );

                    v_direction_unit = v2d_unit(v_direction);

                    v2d v_perp_direction_unit = v2d_perp(
                        v_direction_unit
                    );

                    float closest_distance = MIN(
                        closest_collision_result_left_floor_sensor.distance_from_ray_origin,
                        closest_collision_result_right_floor_sensor.distance_from_ray_origin
                    );

                    v_move_player = v2d_scale(
                        HALF_PLAYER_HEIGHT - closest_distance,
                        v_perp_direction_unit
                    );
                }
                else{ // since they are different lines, try to find the "difference" between them to get an angle "between" the two  angles
                    
                    v2d v_direction_closest_intersecting_line_left_floor_sensor = v2d_sub(
                        closest_collision_result_left_floor_sensor.p1_intersecting_line,
                        closest_collision_result_left_floor_sensor.p2_intersecting_line
                    );

                    v2d v_direction_closest_intersecting_line_right_floor_sensor = v2d_sub(
                        closest_collision_result_right_floor_sensor.p2_intersecting_line,
                        closest_collision_result_right_floor_sensor.p1_intersecting_line
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

                    v_direction_unit = v2d_unit(v_direction);

                    v2d v_perp_direction_unit = v2d_perp(
                        v_direction_unit
                    );

                    float closest_distance = MIN(
                        closest_collision_result_left_floor_sensor.distance_from_ray_origin, 
                        closest_collision_result_right_floor_sensor.distance_from_ray_origin
                    );

                    v_move_player = v2d_scale(
                        HALF_PLAYER_HEIGHT - closest_distance,
                        v_perp_direction_unit
                    );
                    if(v_move_player.y != 0.0f){
                        int x = 0;
                    }
                }
            }

            angles[i].rads = atan2(-v_direction.y, v_direction.x);

            if(angles[i].rads < 0){
                const float maxRads = 3.14 * 2;
                angles[i].rads = maxRads +angles[i].rads;
            }

            // if(groundModes[i] == GROUND_MODE_FLOOR){
            //     if(v_move_player.y > 0){
            //         v_move_player.y *= -1;
            //     }
            // }
            // else if(groundModes[i] == GROUND_MODE_CEILING){
            //     if(v_move_player.y < 0){
            //         v_move_player.y *= -1;
            //     }
            // }
            // else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
            //     if(v_move_player.x < 0){
            //         v_move_player.x *= -1;
            //     }
            // }
            // else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
            //     if(v_move_player.x > 0){
            //         v_move_player.x *= -1;
            //     }
            // }

            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;

        }
        else{ // did not intersect either sensor

            State_util_set(states[i], STATE_IN_AIR);

            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            groundModes[i] = GROUND_MODE_FLOOR;
            angles[i].rads = 0.0f;

        }

        groundModes[i] = util_rads_to_ground_mode(angles[i].rads);

        

        positions[i] = v2d_add(positions[i], v_move_player);


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
}


