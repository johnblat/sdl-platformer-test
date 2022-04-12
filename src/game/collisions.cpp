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
#include "shapeTransformations.h"
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
        printf("p1.x == p2.x");
        return p1.y;
    }
    float m = (p2.y - p1.y)/(p2.x - p1.x);
    float b = p1.y - m * p1.x;
    float y = m * x + b;
    return y;
}

bool ray2dIntersectLineSegment(Ray2d ray, Position p1, Position p2, float &distanceFromRayOrigin){
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


void ray2dSolidRectCollisionSystem(flecs::iter &it, Position *positions, std::vector<Ray2d> *ray2dCollections, Velocity *velocities, StateCurrPrev *states, Angle *angles ){
    
    for(u64 i : it){
        // check against rectangular objects
        // if(velocities[i].y <= 0){
        //     continue;
        // }
        
        State state = STATE_IN_AIR;

        v2d highestIntersectingPoint(
            FLT_MAX,
            FLT_MAX
        );
        v2d p1HighestIntersectingLine;
        v2d p2HighestIntersectingLine;

        for(int vi = 0; vi < ray2dCollections[i].size(); vi++){
            if(velocities[i].y < 0){
                ray2dCollections[i][vi].distance = 16;
            }
            Ray2d rayLocal = ray2dCollections[i].at(vi);

            Ray2d rayGlobal;
            rayGlobal.startingPosition.x = positions[i].x + rayLocal.startingPosition.x;
            rayGlobal.startingPosition.y = positions[i].y + rayLocal.startingPosition.y;
            rayGlobal.distance = rayLocal.distance;


            auto f = it.world().filter<Position, SolidRect>();

            f.each([&](flecs::entity e, Position &position, SolidRect &rect){
                RectVertices rotatedSolidRectVertices = generateRotatedRectVertices(rect, position);


                for(int i = 0; i < 4; i++){
                    v2d p1 = rotatedSolidRectVertices[i];
                    v2d p2 = rotatedSolidRectVertices[(i + 1) % 4];
                    float distanceFromPoint;
                    if(ray2dIntersectLineSegment(rayGlobal, p1, p2, distanceFromPoint)){
                        state = STATE_ON_GROUND;
                        v2d intersectionPoint(
                                rayGlobal.startingPosition.x,
                                rayGlobal.startingPosition.y + distanceFromPoint
                        );
                        if(intersectionPoint.y < highestIntersectingPoint.y){
                            highestIntersectingPoint = intersectionPoint;
                            // Depending on ground mode
                            // this will have to change
                            // for example, when normal
                            // needs tp ensure p1.x < p2.x
                            p1HighestIntersectingLine = p1;
                            p2HighestIntersectingLine = p2;
                        }
                    }
                }

                
                
            });

            auto f2 = it.world().filter<Position, PlatformVertices>();
            f2.each([&](flecs::entity e, Position &position, PlatformVertices &platformVertices){
                for(int i = 0; i < platformVertices.vals.size() - 1; i++){
                    PlatformVertex p1 = platformVertices.vals.at(i);
                    PlatformVertex p2 = platformVertices.vals.at(i+1);
                    v2d v1(p1.x + position.x, p1.y + position.y);
                    v2d v2(p2.x + position.x, p2.y + position.y);
                    float distanceFromPoint;
                    if(ray2dIntersectLineSegment(rayGlobal, v1, v2, distanceFromPoint)){
                        state = STATE_ON_GROUND;
                        v2d intersectionPoint(
                                rayGlobal.startingPosition.x,
                                rayGlobal.startingPosition.y + distanceFromPoint
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
        }
        setState(states[i], state);
        if(highestIntersectingPoint.x != FLT_MAX){
            ray2dCollections[i][0].distance = 32;
            ray2dCollections[i][1].distance = 32;
            positions[i].y = 
                highestIntersectingPoint.y - ray2dCollections[i][0].distance/2;
            v2d intersectingLineVector = 
                p2HighestIntersectingLine - p1HighestIntersectingLine;
            angles[i].rads = atan2(-intersectingLineVector.y, intersectingLineVector.x);
            if(angles[i].rads < 0){
                const float maxRads = 3.14 * 2;
                angles[i].rads = maxRads +angles[i].rads;
            }
            
        }
//        if(state == STATE_IN_AIR){
//            continue;
//        }
        
    }
}