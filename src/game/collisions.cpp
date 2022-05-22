#include "collisions.h"
#include "states.h"
#include "position.h"
#include "flecs.h"
#include "velocity.h"
#include <vector>
#include "ray2d.h"
#include "solid_rect.h"
#include "ints.h"
#define V2D_IMPLEMENTATION
#include "v2d.h"
#include "util.h"
#include <cassert>
#include "shapes.h"
#include "stateProcessing.h"


bool isInRange(float begin, float end, float x){
    if(begin > end){
        swapValues(begin, end, float);
    }
    if(x < begin || x > end){
        return false;
    }
    return true;
}


float getYForXOnLine(Position p1, Position p2, float x){
    if(p2.x < p1.x){
        swapValues(p2, p1, Position);
    }
    if(x < p1.x || x > p2.x){
        assert("getYForXOnLine: X passed must be in the range p1.x to p2.x" && 0);
    }
    if(p1.x == p2.x){
        //printf("p1.x == p2.x");
        return p1.y;
    }
    float m = (p2.y - p1.y)/(p2.x - p1.x);
    float b = p1.y - m * p1.x;
    float y = m * x + b;
    return y;
}

float getXForYOnLine(Position p1, Position p2, float y){
    if(p2.y < p1.y){
        swapValues(p2, p1, Position);
    }
    if(y < p1.y || y > p2.y){
        assert("getYForyOnLine: y passed must be in the range p1.y to p2.y" && 0);
    }
    if(p1.y == p2.y){
        //printf("p1.y == p2.y");
        return p1.x;
    }
    float m = (p2.x - p1.x)/(p2.y - p1.y);
    float b = p1.x - m * p1.y;
    float x = m * y + b;
    return x;
}



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


void sensorsPvsCollisionSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles ){
    
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
        v2d p1HighestIntersectingLine;
        v2d p2HighestIntersectingLine;




        if(velocities[i].y < 0){
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


        

        auto f = it.world().filter<Position, PlatformVertexCollection>();

        f.each([&](flecs::entity e, Position &position, PlatformVertexCollection &platformVertexCollection){
                //walls
            for(int j = 0; j < platformVertexCollection.vals.size() - 1; j++){
                Position p1 = platformVertexCollection.vals.at(j);
                Position p2 = platformVertexCollection.vals.at(j+1);
                v2d v1(p1.x + position.x, p1.y + position.y);
                v2d v2(p2.x + position.x, p2.y + position.y);
                float distanceFromPoint;
                
                // is wall?
                if(p1.x == p2.x){
                    // moving right
                    if(velocities[i].x > 0){
                        if(ray2dIntersectLineSegment(rwRayGlobal, v1, v2, distanceFromPoint, RW_SENSOR)){
                            positions[i].x = v1.x - rwRayGlobal.distance;
                            velocities[i].x = 0;
                            groundSpeeds[i].val = 0.0f;
                        }
                    }
                    // moving left
                    else {
                        if(ray2dIntersectLineSegment(lwRayGlobal, v1, v2, distanceFromPoint, LW_SENSOR)){
                            positions[i].x = v1.x + lwRayGlobal.distance;
                            velocities[i].x = 0;
                            groundSpeeds[i].val = 0.0f;
                        }
                    }
                }
                
            }
        });

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

        f.each([&](flecs::entity e, Position &position, PlatformVertexCollection &platformVertexCollection){
            for(int i = 0; i < platformVertexCollection.vals.size() - 1; i++){
                Position p1 = platformVertexCollection.vals.at(i);
                Position p2 = platformVertexCollection.vals.at(i+1);
                v2d v1(p1.x + position.x, p1.y + position.y);
                v2d v2(p2.x + position.x, p2.y + position.y);
                float distanceFromPoint;

                if(ray2dIntersectLineSegment(lfRayGlobal, v1, v2, distanceFromPoint, LF_SENSOR)){
                    state = STATE_ON_GROUND;
                    v2d intersectionPoint(
                            lfRayGlobal.startingPosition.x,
                            lfRayGlobal.startingPosition.y + distanceFromPoint
                    );
                    if(intersectionPoint.y < highestIntersectingPoint.y){
                        highestIntersectingPoint = intersectionPoint;
                        // Depending on ground mode
                        // this will have to change
                        // for example, when normal
                        // needs tp ensure p1.x < p2.x
                        p1HighestIntersectingLine = v1;
                        p2HighestIntersectingLine = v2;
                    }
                }

                if(ray2dIntersectLineSegment(rfRayGlobal, v1, v2, distanceFromPoint, RF_SENSOR)){
                    state = STATE_ON_GROUND;
                    v2d intersectionPoint(
                            rfRayGlobal.startingPosition.x,
                            rfRayGlobal.startingPosition.y + distanceFromPoint
                    );
                    if(intersectionPoint.y < highestIntersectingPoint.y){
                        highestIntersectingPoint = intersectionPoint;
                        // Depending on ground mode
                        // this will have to change
                        // for example, when normal
                        // needs tp ensure p1.x < p2.x
                        p1HighestIntersectingLine = v1;
                        p2HighestIntersectingLine = v2;
                    }
                }
            }
            
        });
        
        setState(states[i], state);
        if(state == STATE_ON_GROUND){
            sensorCollections[i].rays[LF_SENSOR].distance = 32;
            sensorCollections[i].rays[RF_SENSOR].distance = 32;
            positions[i].y = 
                highestIntersectingPoint.y - sensorCollections[i].rays[LF_SENSOR].distance/2;
            if(p2HighestIntersectingLine.x < p1HighestIntersectingLine.x){
                swapValues(p2HighestIntersectingLine, p1HighestIntersectingLine, Position);
            }
            v2d intersectingLineVector = 
                p2HighestIntersectingLine - p1HighestIntersectingLine;

            angles[i].rads = atan2(-intersectingLineVector.y, intersectingLineVector.x);
            if(angles[i].rads < 0){
                const float maxRads = 3.14 * 2;
                angles[i].rads = maxRads +angles[i].rads;
            }
            
        }
        else {
            sensorCollections[i].rays[LF_SENSOR].distance = 16;
            sensorCollections[i].rays[RF_SENSOR].distance = 16;
        }
    }
}