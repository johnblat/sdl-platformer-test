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
#include "shapes.h"
#include "stateProcessing.h"








bool ray2dIntersectLineSegment(Ray2d ray, Position p1, Position p2, float &distanceFromRayOrigin, SensorType sensorType){
    if(sensorType == LF_SENSOR || sensorType == RF_SENSOR){
        if(!isInRange(p1.x, p2.x, ray.startingPosition.x)){
            return false;
        }
        float y = getYForXOnLine(p1, p2, ray.startingPosition.x);
        distanceFromRayOrigin = y - ray.startingPosition.y;
        if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
            return false;
        }
        return true;
    }
    else if(sensorType == RW_SENSOR){
        if(!isInRange(p1.y, p2.y, ray.startingPosition.y)){
            return false;
        }
        float x = getXForYOnLine(p1, p2, ray.startingPosition.y);
        distanceFromRayOrigin = x - ray.startingPosition.x;
        if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
            return false;
        }
        return true;
    }
    else if(sensorType == LW_SENSOR){
        if(!isInRange(p1.y, p2.y, ray.startingPosition.y)){
            return false;
        }
        float x = getXForYOnLine(p1, p2, ray.startingPosition.y);
        distanceFromRayOrigin = ray.startingPosition.x - x;
        if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
            return false;
        }
        return true;
    }

    return false;

    
    
}









void sensorsPncsCollisionSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles ){
    
    for(u64 i : it){
        
        // set wall sensor height
        if( rads2deg(angles[i].rads) < 5.0 && rads2deg(angles[i].rads) > -5.0){
            sensorCollections[i].rays[RW_SENSOR].startingPosition.y = 8.0f;
            sensorCollections[i].rays[LW_SENSOR].startingPosition.y = 8.0f;
        }
        else {
            sensorCollections[i].rays[RW_SENSOR].startingPosition.y = 0.0f;
            sensorCollections[i].rays[LW_SENSOR].startingPosition.y = 0.0f;
        }

        groundModes[i] = whichGroundMode(angles[i].rads);

        
        
        State state = STATE_IN_AIR;

        v2d highestIntersectingPoint(
            FLT_MAX,
            FLT_MAX
        );

        v2d highestIntersectingLineP1;
        v2d highestIntersectingLineP2;



        // is in air
        if(states[i].currentState == STATE_IN_AIR){
            sensorCollections[i].rays[LF_SENSOR].distance = 16;
            sensorCollections[i].rays[RF_SENSOR].distance = 16;

        }

        

        Ray2d lwRayLocal = sensorCollections[i].rays[LW_SENSOR];
        Ray2d lwRayGlobal;
        lwRayGlobal.startingPosition.x = positions[i].x + lwRayLocal.startingPosition.x;
        lwRayGlobal.startingPosition.y = positions[i].y + lwRayLocal.startingPosition.y;
        lwRayGlobal.distance = lwRayLocal.distance;

        Ray2d rwRayLocal = sensorCollections[i].rays[RW_SENSOR];
        Ray2d rwRayGlobal;
        rwRayGlobal.startingPosition.x = positions[i].x + rwRayLocal.startingPosition.x;
        rwRayGlobal.startingPosition.y = positions[i].y + rwRayLocal.startingPosition.y;
        rwRayGlobal.distance = rwRayLocal.distance;


        

        auto f = it.world().filter<Position, PlatformNodeCollection>();

        // NOTE!!!
        // COMMENTED OUT TO IGNORE WALL SENSORS FOR NOW

        // f.each([&](flecs::entity e, Position &position, PlatformNodeCollection &platformVertexCollection){
        //         //walls
        //     size_t lenCollection = platformVertexCollection.vals.size();
        //     for(int j = 0; j < lenCollection - 1; j++){
        //         Position p1 = platformVertexCollection.vals.at(j);
        //         Position p2 = platformVertexCollection.vals.at(j+1);
        //         v2d v1(p1.x + position.x, p1.y + position.y);
        //         v2d v2(p2.x + position.x, p2.y + position.y);
        //         float distanceFromPoint;
                
        //         // is wall?
        //         if(p1.x == p2.x){
        //             // moving right
        //             if(velocities[i].x > 0){
        //                 if(ray2dIntersectLineSegment(rwRayGlobal, v1, v2, distanceFromPoint, RW_SENSOR)){
        //                     positions[i].x = v1.x - rwRayGlobal.distance;
        //                     velocities[i].x = 0;
        //                     groundSpeeds[i].val = 0.0f;
        //                 }
        //             }
        //             // moving left
        //             else {
        //                 if(ray2dIntersectLineSegment(lwRayGlobal, v1, v2, distanceFromPoint, LW_SENSOR)){
        //                     positions[i].x = v1.x + lwRayGlobal.distance;
        //                     velocities[i].x = 0;
        //                     groundSpeeds[i].val = 0.0f;
        //                 }
        //             }
        //         }
                
        //     }
        // });

        Ray2d lfSesnorRayLocal = sensorCollections[i].rays[LF_SENSOR];
        Ray2d lfRayGlobal;
        lfRayGlobal.startingPosition.x = positions[i].x + lfSesnorRayLocal.startingPosition.x;
        lfRayGlobal.startingPosition.y = positions[i].y + lfSesnorRayLocal.startingPosition.y;
        lfRayGlobal.distance = lfSesnorRayLocal.distance;

        Ray2d rfRayLocal = sensorCollections[i].rays[RF_SENSOR];
        Ray2d rfRayGlobal;
        rfRayGlobal.startingPosition.x = positions[i].x + rfRayLocal.startingPosition.x;
        rfRayGlobal.startingPosition.y = positions[i].y + rfRayLocal.startingPosition.y;
        rfRayGlobal.distance = rfRayLocal.distance;

        f.each([&](flecs::entity e, Position &position, PlatformNodeCollection &platformVertexCollection){
            size_t len = platformVertexCollection.vals.size();
            for(int v = 0; v < len - 1; v++){
                //breakOnCondition(groundModes[i] == RIGHT_WALL_GM || groundModes[i] == LEFT_WALL_GM);
                
                Position p1 = platformVertexCollection.vals.at(v);
                Position p2 = platformVertexCollection.vals.at(v+1);
                v2d v1(p1.x + position.x, p1.y + position.y);
                v2d v2(p2.x + position.x, p2.y + position.y);
                v2d r1 = v1;
                v2d r2 = v2;
                if(groundModes[i] == RIGHT_WALL_GM && states[i].currentState == STATE_ON_GROUND){
                    r1 = v2dRotate90DegreesCW(v1, positions[i]);
                    r2 = v2dRotate90DegreesCW(v2, positions[i]);
                }
                else if(groundModes[i] == LEFT_WALL_GM && states[i].currentState == STATE_ON_GROUND){
                    r1 = v2dRotate90DegreesCCW(v1, positions[i]);
                    r2 = v2dRotate90DegreesCCW(v2, positions[i]);
                }
                else if(groundModes[i] == CEILING_GM && states[i].currentState == STATE_ON_GROUND){
                    r1 = v2dRotate180Degrees(v1, positions[i]);
                    r2 = v2dRotate180Degrees(v2, positions[i]);
                }

                float distanceFromPoint;

                if(ray2dIntersectLineSegment(lfRayGlobal, r1, r2, distanceFromPoint, LF_SENSOR)){
                   //breakOnCondition(groundModes[i] == CEILING_GM);
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

                if(ray2dIntersectLineSegment(rfRayGlobal, r1, r2, distanceFromPoint, RF_SENSOR)){
                    //breakOnCondition(groundModes[i] == CEILING_GM);
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


        if(groundModes[i] == RIGHT_WALL_GM && states[i].currentState == STATE_ON_GROUND){
            highestIntersectingPoint = v2dRotate90DegreesCCW(highestIntersectingPoint, positions[i]);
            highestIntersectingLineP1 = v2dRotate90DegreesCCW(highestIntersectingLineP1, positions[i]);
            highestIntersectingLineP2 = v2dRotate90DegreesCCW(highestIntersectingLineP2, positions[i]);
        }
        else if(groundModes[i] == LEFT_WALL_GM && states[i].currentState == STATE_ON_GROUND){
            highestIntersectingPoint = v2dRotate90DegreesCW(highestIntersectingPoint, positions[i]);
            highestIntersectingLineP1 = v2dRotate90DegreesCW(highestIntersectingLineP1, positions[i]);
            highestIntersectingLineP2 = v2dRotate90DegreesCW(highestIntersectingLineP2, positions[i]);
        }
        else if(groundModes[i] == CEILING_GM && states[i].currentState == STATE_ON_GROUND){
            highestIntersectingPoint = v2dRotate180Degrees(highestIntersectingPoint, positions[i]);
            highestIntersectingLineP1 = v2dRotate180Degrees(highestIntersectingLineP1, positions[i]);
            highestIntersectingLineP2 = v2dRotate180Degrees(highestIntersectingLineP2, positions[i]);
        }
        
        setState(states[i], state);
        if(state == STATE_ON_GROUND){
            sensorCollections[i].rays[LF_SENSOR].distance = 32;
            sensorCollections[i].rays[RF_SENSOR].distance = 32;
            if(groundModes[i] == FLOOR_GM){
                positions[i].y = 
                    highestIntersectingPoint.y - sensorCollections[i].rays[LF_SENSOR].distance/2;
                if(highestIntersectingLineP2.x < highestIntersectingLineP1.x){
                    swapValues(highestIntersectingLineP2, highestIntersectingLineP1, Position);
                }
            }
            else if(groundModes[i] == CEILING_GM){
                positions[i].y = 
                    highestIntersectingPoint.y +sensorCollections[i].rays[LF_SENSOR].distance/2;
                if(highestIntersectingLineP2.x > highestIntersectingLineP1.x){
                    swapValues(highestIntersectingLineP2, highestIntersectingLineP1, Position);
                }
            }
            else if(groundModes[i] == LEFT_WALL_GM){
                positions[i].x = 
                    highestIntersectingPoint.x + sensorCollections[i].rays[LF_SENSOR].distance/2;
                if(highestIntersectingLineP2.y < highestIntersectingLineP1.y){
                    swapValues(highestIntersectingLineP2, highestIntersectingLineP1, Position);
                }
            }
            else if(groundModes[i] == RIGHT_WALL_GM){
                positions[i].x = 
                    highestIntersectingPoint.x - sensorCollections[i].rays[LF_SENSOR].distance/2;
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
            sensorCollections[i].rays[LF_SENSOR].distance = 16;
            sensorCollections[i].rays[RF_SENSOR].distance = 16;
            groundModes[i] = FLOOR_GM;
            angles[i].rads = 0.0f;
        }
    }
}