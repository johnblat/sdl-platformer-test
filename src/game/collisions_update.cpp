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
        ray2d_left_wall_world.startingPosition = util_local_to_world_position(ray2d_left_wall_local.startingPosition, positions[i]);
        ray2d_left_wall_world.distance = ray2d_left_wall_local.distance;

        Ray2d ray2d_right_wall_local, ray2d_right_wall_world; 
        ray2d_right_wall_local = sensorCollections[i].rays[SENSOR_RIGHT_WALL];
        ray2d_right_wall_world.startingPosition = util_local_to_world_position(ray2d_right_wall_local.startingPosition, positions[i]);
        ray2d_right_wall_world.distance = ray2d_right_wall_local.distance;

        auto f = it.world().filter<Position, PlatformPath>();

        f.each([&](flecs::entity e, Position &platformPath_position, PlatformPath &platformPath){
                //walls
            for(int j = 0; j < platformPath.nodes.size() - 1; j++){
                Position local_position_node_current = platformPath.nodes.at(j);
                Position local_position_node_next = platformPath.nodes.at(j+1);

                v2d world_position_node_current = util_local_to_world_position(local_position_node_current, platformPath_position);
                v2d world_position_node_next = util_local_to_world_position(local_position_node_next, platformPath_position);

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
                
                // is wall?
                if(true){
                    // moving right
                    if(velocities[i].x > 0){
                       if(collisions_Ray2d_intersects_line_segment(ray2d_right_wall_world, world_position_node_current_rotated, world_position_node_next_rotated, intersection_distance_from_ray_origin, SENSOR_RIGHT_WALL)){
                           positions[i].x = world_position_node_current.x - SENSORS_DEFAULT_WALL_DISTANCE;
                            velocities[i].x = 0;
                            groundSpeeds[i].val = 0.0f;
                            velocities[i].x = 0.0f;
                        }
                    }
                    // moving left
                    else if(velocities[i].x < 0) {
                        if(collisions_Ray2d_intersects_line_segment(ray2d_left_wall_world, world_position_node_current_rotated, world_position_node_next_rotated, intersection_distance_from_ray_origin, SENSOR_LEFT_WALL)){
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
            sensorCollections[i].rays[SENSOR_RIGHT_WALL].startingPosition.y = FLAT_ENOUGH_WALL_SENSOR_HEIGHT;
            sensorCollections[i].rays[SENSOR_LEFT_WALL].startingPosition.y = FLAT_ENOUGH_WALL_SENSOR_HEIGHT;
        }
        else {
            sensorCollections[i].rays[SENSOR_RIGHT_WALL].startingPosition.y = NOT_FLAT_WALL_SENSOR_HEIGHT;
            sensorCollections[i].rays[SENSOR_LEFT_WALL].startingPosition.y = NOT_FLAT_WALL_SENSOR_HEIGHT;
        }
    }
}

void collisions_GroundMode_update_from_Angle_System(flecs::iter &it, GroundMode *groundModes, Angle *angles){
    for(u64 i : it){
        groundModes[i] = util_rads_to_ground_mode(angles[i].rads);
    }
}



// void collisions_Angle_update_from_intersecting_sensors(flecs::iter &it, Angle)

void collisions_Sensors_PlatformPaths_update_Angle_System(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles ){
    
    for(u64 i : it){
        
        State state = STATE_IN_AIR;

        v2d highestIntersectingPoint(
            FLT_MAX,
            FLT_MAX
        );

        v2d highestIntersectingLineP1;
        v2d highestIntersectingLineP2;



        // is in air
        if(states[i].currentState == STATE_IN_AIR){
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

        }

        Ray2d lfSesnorRayLocal = sensorCollections[i].rays[SENSOR_LEFT_FLOOR];
        Ray2d lfRayGlobal;
        lfRayGlobal.startingPosition.x = positions[i].x + lfSesnorRayLocal.startingPosition.x;
        lfRayGlobal.startingPosition.y = positions[i].y + lfSesnorRayLocal.startingPosition.y;
        lfRayGlobal.distance = lfSesnorRayLocal.distance;

        Ray2d rfRayLocal = sensorCollections[i].rays[SENSOR_RIGHT_FLOOR];
        Ray2d rfRayGlobal;
        rfRayGlobal.startingPosition.x = positions[i].x + rfRayLocal.startingPosition.x;
        rfRayGlobal.startingPosition.y = positions[i].y + rfRayLocal.startingPosition.y;
        rfRayGlobal.distance = rfRayLocal.distance;

        auto f = it.world().filter<Position, PlatformPath>();

        f.each([&](flecs::entity e, Position &position, PlatformPath &platformPath){
            size_t len = platformPath.nodes.size();
            for(int v = 0; v < len - 1; v++){                
                Position p1 = platformPath.nodes.at(v);
                Position p2 = platformPath.nodes.at(v+1);
                v2d v1(p1.x + position.x, p1.y + position.y);
                v2d v2(p2.x + position.x, p2.y + position.y);
                v2d r1 = v2d_rotate(v1, positions[i], angles[i].rads);
                v2d r2 = v2d_rotate(v2, positions[i], angles[i].rads);
                
                float distanceFromPoint;

                if(collisions_Ray2d_intersects_line_segment(lfRayGlobal, r1, r2, distanceFromPoint, SENSOR_LEFT_FLOOR)){
                    state = STATE_ON_GROUND;
                    v2d intersectionPoint(
                            lfRayGlobal.startingPosition.x,
                            lfRayGlobal.startingPosition.y + distanceFromPoint
                    );
                    if(intersectionPoint.y < highestIntersectingPoint.y){
                        highestIntersectingPoint = intersectionPoint;
                        highestIntersectingLineP1 = r1;
                        highestIntersectingLineP2 = r2;
                    }
                }

                if(collisions_Ray2d_intersects_line_segment(rfRayGlobal, r1, r2, distanceFromPoint, SENSOR_RIGHT_FLOOR)){
                    state = STATE_ON_GROUND;
                    v2d intersectionPoint(
                            rfRayGlobal.startingPosition.x,
                            rfRayGlobal.startingPosition.y + distanceFromPoint
                    );
                    if(intersectionPoint.y < highestIntersectingPoint.y){
                        highestIntersectingPoint = intersectionPoint;
                        highestIntersectingLineP1 = r1;
                        highestIntersectingLineP2 = r2;
                    }
                }

                
                
                
            } 
            
        });

        // rotate highest points back to their normal state

        highestIntersectingPoint = v2d_rotate(highestIntersectingPoint, positions[i], -angles[i].rads);
        highestIntersectingLineP1 = v2d_rotate(highestIntersectingLineP1, positions[i], -angles[i].rads);
        highestIntersectingLineP2 = v2d_rotate(highestIntersectingLineP2, positions[i], -angles[i].rads);

        
        State_util_set(states[i], state);
        if(state == STATE_ON_GROUND){
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            if(groundModes[i] == GROUND_MODE_FLOOR){
                if(highestIntersectingLineP2.x < highestIntersectingLineP1.x){
                    swapValues(highestIntersectingLineP2, highestIntersectingLineP1, Position);
                }
            }
            else if(groundModes[i] == GROUND_MODE_CEILING){
                if(highestIntersectingLineP2.x > highestIntersectingLineP1.x){
                    swapValues(highestIntersectingLineP2, highestIntersectingLineP1, Position);
                }
            }
            else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
                if(highestIntersectingLineP2.y < highestIntersectingLineP1.y){
                    swapValues(highestIntersectingLineP2, highestIntersectingLineP1, Position);
                }
            }
            else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
                if(highestIntersectingLineP2.y > highestIntersectingLineP1.y){
                    swapValues(highestIntersectingLineP2, highestIntersectingLineP1, Position);
                }
            }
            
            v2d intersectingLineVector = 
                highestIntersectingLineP2 - highestIntersectingLineP1;

            angles[i].rads = atan2(-intersectingLineVector.y, intersectingLineVector.x);
            if(angles[i].rads < 0){
                const float maxRads = 3.14 * 2;
                angles[i].rads = maxRads +angles[i].rads;
            }
            
        }
        else {
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
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

        v2d highestIntersectingPoint(
            FLT_MAX,
            FLT_MAX
        );

        v2d highestIntersectingLineP1;
        v2d highestIntersectingLineP2;



        // is in air
        if(states[i].currentState == STATE_IN_AIR){
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

        }

        Ray2d lfSesnorRayLocal = sensorCollections[i].rays[SENSOR_LEFT_FLOOR];
        Ray2d lfRayGlobal;
        lfRayGlobal.startingPosition.x = positions[i].x + lfSesnorRayLocal.startingPosition.x;
        lfRayGlobal.startingPosition.y = positions[i].y + lfSesnorRayLocal.startingPosition.y;
        lfRayGlobal.distance = lfSesnorRayLocal.distance;

        Ray2d rfRayLocal = sensorCollections[i].rays[SENSOR_RIGHT_FLOOR];
        Ray2d rfRayGlobal;
        rfRayGlobal.startingPosition.x = positions[i].x + rfRayLocal.startingPosition.x;
        rfRayGlobal.startingPosition.y = positions[i].y + rfRayLocal.startingPosition.y;
        rfRayGlobal.distance = rfRayLocal.distance;

        float closest_distance_from_point;

        auto f = it.world().filter<Position, PlatformPath>();

        f.each([&](flecs::entity e, Position &position, PlatformPath &platformPath){
            size_t len = platformPath.nodes.size();
            for(int v = 0; v < len - 1; v++){                
                Position p1 = platformPath.nodes.at(v);
                Position p2 = platformPath.nodes.at(v+1);
                v2d v1(p1.x + position.x, p1.y + position.y);
                v2d v2(p2.x + position.x, p2.y + position.y);
                v2d r1 = v2d_rotate(v1, positions[i], angles[i].rads);
                v2d r2 = v2d_rotate(v2, positions[i], angles[i].rads);
               
                float distance_from_point;

                if(collisions_Ray2d_intersects_line_segment(lfRayGlobal, r1, r2, distance_from_point, SENSOR_LEFT_FLOOR)){
                    state = STATE_ON_GROUND;
                    v2d intersectionPoint(
                            lfRayGlobal.startingPosition.x,
                            lfRayGlobal.startingPosition.y + distance_from_point
                    );
                    if(intersectionPoint.y < highestIntersectingPoint.y){
                        highestIntersectingPoint = intersectionPoint;
                        highestIntersectingLineP1 = r1;
                        highestIntersectingLineP2 = r2;
                        closest_distance_from_point = distance_from_point;
                    }
                }

                if(collisions_Ray2d_intersects_line_segment(rfRayGlobal, r1, r2, distance_from_point, SENSOR_RIGHT_FLOOR)){
                    state = STATE_ON_GROUND;
                    v2d intersectionPoint(
                            rfRayGlobal.startingPosition.x,
                            rfRayGlobal.startingPosition.y + distance_from_point
                    );
                    if(intersectionPoint.y < highestIntersectingPoint.y){
                        highestIntersectingPoint = intersectionPoint;
                        highestIntersectingLineP1 = r1;
                        highestIntersectingLineP2 = r2;
                        closest_distance_from_point = distance_from_point;

                    }
                }
            } 
        });

        // rotate highest points back to their normal state
        highestIntersectingPoint = v2d_rotate(highestIntersectingPoint, positions[i], -angles[i].rads);
        highestIntersectingLineP1 = v2d_rotate(highestIntersectingLineP1, positions[i], -angles[i].rads);
        highestIntersectingLineP2 = v2d_rotate(highestIntersectingLineP2, positions[i], -angles[i].rads);
        
        State_util_set(states[i], state);
        if(state == STATE_ON_GROUND){
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_GROUND_DISTANCE;
            if(groundModes[i] == GROUND_MODE_FLOOR){
                positions[i].y -= (HALF_PLAYER_HEIGHT - closest_distance_from_point);
                
            }
            else if(groundModes[i] == GROUND_MODE_CEILING){
                positions[i].y +=  (HALF_PLAYER_HEIGHT - closest_distance_from_point);
                
            }
            else if(groundModes[i] == GROUND_MODE_LEFT_WALL){
                positions[i].x += (HALF_PLAYER_HEIGHT - closest_distance_from_point);
                
            }
            else if(groundModes[i] == GROUND_MODE_RIGHT_WALL){
                positions[i].x -= (HALF_PLAYER_HEIGHT - closest_distance_from_point);
               
            }
            
            
        }
        else {
            sensorCollections[i].rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            sensorCollections[i].rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;
            groundModes[i] = GROUND_MODE_FLOOR;
            angles[i].rads = 0.0f;
        }
    }
}


