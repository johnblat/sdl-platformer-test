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
#include "camera.h"


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

void collisions_Sensors_wall_sensors_set_distance_from_velocity_System(flecs::iter &it, Sensors *sensorCollections, Velocity *velocities, GroundMode *groundModes){

    for(u64 i : it){
        sensorCollections[i].rays[SENSOR_LEFT_WALL].distance = SENSORS_DEFAULT_WALL_DISTANCE;
        sensorCollections[i].rays[SENSOR_RIGHT_WALL].distance = SENSORS_DEFAULT_WALL_DISTANCE;

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
    

    for(u64 i : it){

        Ray2d left_wall_sensor_ray_world = ray2d_local_to_world(
            positions[i], 
            sensorCollections[i].rays[SENSOR_LEFT_WALL]
        );
        
        Ray2d right_wall_sensor_ray_world = ray2d_local_to_world(
            positions[i], 
            sensorCollections[i].rays[SENSOR_RIGHT_WALL]
        );

        CollisionResultRay2dIntersectLine closest_collision_result_left_wall;
        closest_collision_result_left_wall.did_intersect = false;

        CollisionResultRay2dIntersectLine closest_collision_result_right_wall;
        closest_collision_result_right_wall.did_intersect = false;


        auto f = it.world().filter<Position, PlatformPath>();

        f.each([&](flecs::entity e, Position &platform_path_world_position, PlatformPath &platformPath){
                //walls
            for(int j = 0; j < platformPath.nodes.size() - 1; j++){
                v2d p1_local_platform_line = platformPath.nodes.at(j);
                v2d p2_local_platform_line = platformPath.nodes.at(j+1);

                v2d p1_world_platform_line = util_v2d_local_to_world(
                    p1_local_platform_line, 
                    platform_path_world_position 
                );
                v2d p2_world_platform_line = util_v2d_local_to_world(
                    p2_local_platform_line, 
                    platform_path_world_position 
                );

                if(velocities[i].x < 0){
                    CollisionResultRay2dIntersectLine current_collision_result_left_wall = collisions_Ray2d_rotated_intersects_line_segment_result(
                        left_wall_sensor_ray_world,
                        SENSOR_LEFT_WALL,
                        angles[i].rads,
                        positions[i],
                        p1_world_platform_line,
                        p2_world_platform_line
                    );

                    if(current_collision_result_left_wall.did_intersect){
                        if(!closest_collision_result_left_wall.did_intersect){
                            closest_collision_result_left_wall = current_collision_result_left_wall;
                        }
                        else if(current_collision_result_left_wall.distance_from_ray_origin < closest_collision_result_left_wall.distance_from_ray_origin){
                            closest_collision_result_left_wall = current_collision_result_left_wall;
                        }
                    }
                    
                }
                else if(velocities[i].x > 0){
                    CollisionResultRay2dIntersectLine current_collision_result_right_wall = collisions_Ray2d_rotated_intersects_line_segment_result(
                        right_wall_sensor_ray_world,
                        SENSOR_RIGHT_WALL,
                        angles[i].rads,
                        positions[i],
                        p1_world_platform_line,
                        p2_world_platform_line
                    );

                    if(current_collision_result_right_wall.did_intersect){
                        if(!closest_collision_result_left_wall.did_intersect){
                            closest_collision_result_right_wall = current_collision_result_right_wall;
                        }
                        else if(current_collision_result_right_wall.distance_from_ray_origin < closest_collision_result_right_wall.distance_from_ray_origin){
                            closest_collision_result_right_wall = current_collision_result_right_wall;
                        }
                    }
                }
            }
        });

        if(velocities[i].x < 0 && closest_collision_result_left_wall.did_intersect){
            positions[i].x = closest_collision_result_left_wall.p_world_intersection.x + SENSORS_DEFAULT_WALL_DISTANCE;
            velocities[i].x = 0.0f;
            groundSpeeds[i].val = 0.0f;
        }
        else if(velocities[i].x > 0 && closest_collision_result_right_wall.did_intersect){
            positions[i].x = closest_collision_result_right_wall.p_world_intersection.x - (SENSORS_DEFAULT_WALL_DISTANCE);
            velocities[i].x = 0.0f;
            groundSpeeds[i].val = 0.0f;
        }
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


void internal_collisions_ray2d_find_closest_center_floor_intersection_on_platform_paths(
    flecs::world &world, 
    Ray2d center_floor_sensor_ray_world, 
    float rotation_in_rads, 
    v2d p_rotation_origin, 
    CollisionResultRay2dIntersectLine &closest_collision_result_center_floor
){
    closest_collision_result_center_floor.did_intersect = false;

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

        
            CollisionResultRay2dIntersectLine current_collision_result_center_floor = collisions_Ray2d_rotated_intersects_line_segment_result(
                center_floor_sensor_ray_world,
                SENSOR_CENTER_FLOOR,
                rotation_in_rads,
                p_rotation_origin,
                p1_world_platform_line,
                p2_world_platform_line
            );
            

            if(current_collision_result_center_floor.did_intersect){

                if(!closest_collision_result_center_floor.did_intersect){ // no closest one yet
                    closest_collision_result_center_floor = current_collision_result_center_floor;
                }
                else if(current_collision_result_center_floor.distance_from_ray_origin < closest_collision_result_center_floor.distance_from_ray_origin){
                    closest_collision_result_center_floor = current_collision_result_center_floor;
                }
            }
        } 
    });

}

void collisions_Sensors_PlatformPaths_update_Angle_System(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles ){
    
    for(u64 i : it){
        

        CollisionResultRay2dIntersectLine closest_collision_result_center_floor_sensor;

        // ensure height is correct for in air collision check
        if(states[i].currentState == STATE_IN_AIR){
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

            if(velocities[i].y < 0){ // going up
                continue; // don't check
            }

        }


        Ray2d center_floor_sensor_ray_world = ray2d_local_to_world(
            positions[i], 
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR]
        );

        flecs::world world = it.world();


        internal_collisions_ray2d_find_closest_center_floor_intersection_on_platform_paths(
            world,
            center_floor_sensor_ray_world,
            angles[i].rads,
            positions[i],
            closest_collision_result_center_floor_sensor
        );


        

        // the direction of vector the player will be standing on
        // used to determine angle
        v2d v_direction;

        if(closest_collision_result_center_floor_sensor.did_intersect){
            // ALIGN LINES TO FACE CORRECT WAY
            {
            if(groundModes[i] == GROUND_MODE_FLOOR){
                if(closest_collision_result_center_floor_sensor.p2_intersecting_line.x < closest_collision_result_center_floor_sensor.p1_intersecting_line.x){
                    swapValues(
                        closest_collision_result_center_floor_sensor.p2_intersecting_line, 
                        closest_collision_result_center_floor_sensor.p1_intersecting_line, 
                        Position
                    );
                }
            }
            else if(groundModes[i] == GROUND_MODE_CEILING){
                if(closest_collision_result_center_floor_sensor.p2_intersecting_line.x > closest_collision_result_center_floor_sensor.p1_intersecting_line.x){
                    swapValues(
                        closest_collision_result_center_floor_sensor.p2_intersecting_line, 
                        closest_collision_result_center_floor_sensor.p1_intersecting_line, 
                        Position
                    );
                }
            }
            else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
                if(closest_collision_result_center_floor_sensor.p2_intersecting_line.y < closest_collision_result_center_floor_sensor.p1_intersecting_line.y){
                    swapValues(
                        closest_collision_result_center_floor_sensor.p2_intersecting_line, 
                        closest_collision_result_center_floor_sensor.p1_intersecting_line, 
                        Position
                    );
                }
            }
            else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
                if(closest_collision_result_center_floor_sensor.p2_intersecting_line.y > closest_collision_result_center_floor_sensor.p1_intersecting_line.y){
                    swapValues(
                        closest_collision_result_center_floor_sensor.p2_intersecting_line, 
                        closest_collision_result_center_floor_sensor.p1_intersecting_line, 
                        Position
                    );
                }
            }
            }

            v_direction = v2d_sub(
                closest_collision_result_center_floor_sensor.p2_intersecting_line,
                closest_collision_result_center_floor_sensor.p1_intersecting_line
            );

            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;

            float angle_in_rads_of_line_direction = atan2(-v_direction.y, v_direction.x);

            if(angle_in_rads_of_line_direction < 0){
                const float maxRads = 3.14 * 2;
                angle_in_rads_of_line_direction = maxRads + angle_in_rads_of_line_direction;
            }
            // rotate player around intersection point
            float rotation_in_rads_needed_to_align_player = angles[i].rads - angle_in_rads_of_line_direction;

            if(rotation_in_rads_needed_to_align_player < 0){
                const float maxRads = 3.14 * 2;
                rotation_in_rads_needed_to_align_player = maxRads + rotation_in_rads_needed_to_align_player;
            }

            positions[i] = v2d_rotate(positions[i], closest_collision_result_center_floor_sensor.p_world_intersection, rotation_in_rads_needed_to_align_player);

            angles[i].rads = angle_in_rads_of_line_direction;

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
        

        // ensure height is correct for in air collision check
        if(states[i].currentState == STATE_IN_AIR){
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

            if(velocities[i].y < 0){ // going up
                gCameraPosition = positions[i];
                gCameraPosition.y += HALF_PLAYER_HEIGHT;
                angles[i].rads = 0.0f;
                continue; // don't check
                
            } // eventually, need to check ceiling sensors in this case

        }

        CollisionResultRay2dIntersectLine closest_collision_result_center_floor_sensor;

        // ensure height is correct for in air collision check
        if(states[i].currentState == STATE_IN_AIR){
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

        }


        Ray2d center_floor_sensor_ray_world = ray2d_local_to_world(
            positions[i], 
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR]
        );

        flecs::world world = it.world();

        internal_collisions_ray2d_find_closest_center_floor_intersection_on_platform_paths(
            world,
            center_floor_sensor_ray_world,
            angles[i].rads,
            positions[i],
            closest_collision_result_center_floor_sensor
        );

        // HANDLE COLLISIONS IF ANY

        // the direction of vector the player will be standing on
        // used to determine angle
        v2d v_direction;
        v2d v_direction_unit;
        v2d v_move_player(0.0f, 0.0f);

        if(closest_collision_result_center_floor_sensor.did_intersect){

            State_util_set(states[i], STATE_ON_GROUND);

            // ALIGN INTERSECTING LINES WITH GROUND MODE
            {
            if(groundModes[i] == GROUND_MODE_FLOOR){
                if(closest_collision_result_center_floor_sensor.p2_intersecting_line.x < closest_collision_result_center_floor_sensor.p1_intersecting_line.x){
                    swapValues(
                        closest_collision_result_center_floor_sensor.p2_intersecting_line, 
                        closest_collision_result_center_floor_sensor.p1_intersecting_line, 
                        Position
                    );
                }
            }
            else if(groundModes[i] == GROUND_MODE_CEILING){
                if(closest_collision_result_center_floor_sensor.p2_intersecting_line.x > closest_collision_result_center_floor_sensor.p1_intersecting_line.x){
                    swapValues(
                        closest_collision_result_center_floor_sensor.p2_intersecting_line, 
                        closest_collision_result_center_floor_sensor.p1_intersecting_line, 
                        Position
                    );
                }
            }
            else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
                if(closest_collision_result_center_floor_sensor.p2_intersecting_line.y < closest_collision_result_center_floor_sensor.p1_intersecting_line.y){
                    swapValues(
                        closest_collision_result_center_floor_sensor.p2_intersecting_line, 
                        closest_collision_result_center_floor_sensor.p1_intersecting_line, 
                        Position
                    );
                }
            }
            else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
                if(closest_collision_result_center_floor_sensor.p2_intersecting_line.y > closest_collision_result_center_floor_sensor.p1_intersecting_line.y){
                    swapValues(
                        closest_collision_result_center_floor_sensor.p2_intersecting_line, 
                        closest_collision_result_center_floor_sensor.p1_intersecting_line, 
                        Position
                    );
                }
            }
            }

            v_direction = v2d_sub(
                closest_collision_result_center_floor_sensor.p2_intersecting_line,
                closest_collision_result_center_floor_sensor.p1_intersecting_line
            );

            v_direction_unit = v2d_unit(v_direction);

            v2d v_perp_direction_unit = v2d_perp(
                v_direction_unit
            );

            v_move_player = v2d_scale(
                HALF_PLAYER_HEIGHT - closest_collision_result_center_floor_sensor.distance_from_ray_origin ,
                v_perp_direction_unit
            );

            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;

            // set camera positoin as the intersection point for camera "stability"
            // setting camera to player position would be jittery around corners since his position is being rotated around intersection point
            gCameraPosition = closest_collision_result_center_floor_sensor.p_world_intersection;
        }
        else{ // did not intersect either sensor

            State_util_set(states[i], STATE_IN_AIR);

            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            groundModes[i] = GROUND_MODE_FLOOR;
            angles[i].rads = 0.0f;

            // camera set here as player position - half height in y to be closer to where an intersection point would bea
            gCameraPosition = positions[i];
            gCameraPosition.y += HALF_PLAYER_HEIGHT;

        }

        groundModes[i] = util_rads_to_ground_mode(angles[i].rads);

        positions[i] = v2d_add(positions[i], v_move_player);    
        
    }
}


