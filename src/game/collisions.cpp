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


const Position lfSensorFloorGroundModeStartingPosition = {-8.0f,0.0f};
const Position rfSensorFloorGroundModeStartingPosition = {8.0f,0.0f};
const Position lwSensorFloorGroundModeStartingPosition = {0.0f,0.0f};
const Position rwSensorFloorGroundModeStartingPosition = {0.0f,0.0f};

const Position lfSensorLeftWallGroundModeStartingPosition = {0.0f,-8.0f};
const Position rfSensorLeftWallGroundModeStartingPosition = {0.0f,8.0f};
const Position lwSensorLeftWallGroundModeStartingPosition = {0.0f,0.0f};
const Position rwSensorLeftWallGroundModeStartingPosition = {0.0f,0.0f};

const Position lfSensorRightWallGroundModeStartingPosition = {0.0f,8.0f};
const Position rfSensorRightWallGroundModeStartingPosition = {0.0f,-8.0f};
const Position lwSensorRightWallGroundModeStartingPosition = {0.0f,0.0f};
const Position rwSensorRightWallGroundModeStartingPosition = {0.0f,0.0f};


void positionSensorsBasedOnGroundMode(Sensors &sensors, GroundMode groundMode){
    if(groundMode == FLOOR_GM){
        sensors.rays[LF_SENSOR].startingPosition = lfSensorFloorGroundModeStartingPosition;
        sensors.rays[RF_SENSOR].startingPosition = rfSensorFloorGroundModeStartingPosition;
        sensors.rays[LW_SENSOR].startingPosition = lwSensorFloorGroundModeStartingPosition;
        sensors.rays[RW_SENSOR].startingPosition = rwSensorFloorGroundModeStartingPosition;
    }
    if(groundMode == LEFT_WALL_GM){
        sensors.rays[LF_SENSOR].startingPosition = lfSensorLeftWallGroundModeStartingPosition;
        sensors.rays[RF_SENSOR].startingPosition = rfSensorLeftWallGroundModeStartingPosition;
        sensors.rays[LW_SENSOR].startingPosition = lwSensorLeftWallGroundModeStartingPosition;
        sensors.rays[RF_SENSOR].startingPosition = rwSensorLeftWallGroundModeStartingPosition;
    }
    if(groundMode == RIGHT_WALL_GM){
        sensors.rays[LF_SENSOR].startingPosition = lfSensorRightWallGroundModeStartingPosition;
        sensors.rays[RF_SENSOR].startingPosition = rfSensorRightWallGroundModeStartingPosition;
        sensors.rays[LW_SENSOR].startingPosition = lwSensorRightWallGroundModeStartingPosition;
        sensors.rays[RF_SENSOR].startingPosition = rwSensorRightWallGroundModeStartingPosition;
    }

}




void sensorsPVCsCollisionSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections, Velocity *velocities, GroundSpeed *groundSpeeds, GroundMode *groundModes, StateCurrPrev *states, Angle *angles ){
    
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


        

        auto f = it.world().filter<Position, PlatformVertexCollection>();

        // NOTE!!!
        // COMMENTED OUT TO IGNORE WALL SENSORS FOR NOW

        // f.each([&](flecs::entity e, Position &position, PlatformVertexCollection &platformVertexCollection){
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

        f.each([&](flecs::entity e, Position &position, PlatformVertexCollection &platformVertexCollection){
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