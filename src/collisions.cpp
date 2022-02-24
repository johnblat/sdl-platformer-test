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

struct RectVertices{
    v2d v1, v2, v3, v4;

    RectVertices(v2d V1, v2d V2, v2d V3, v2d V4) : v1(V1), v2(V2), v3(V3), v4(V4) {}
    RectVertices() :
        v1(0, 0),
        v2(0, 0),
        v3(0, 0),
        v4(0, 0) {}

    v2d &operator[](const u32 i){
        return (&v1)[i];
    }

    const v2d &operator[](const u32 i) const {
        return (&v1)[i];
    }
};

RectVertices generateRotatedRectVertices(SolidRect rect, Position center){
    RectVertices initialVertices(
            v2d(center.x - rect.w/2, center.y - rect.h/2),
            v2d(center.x + rect.w/2, center.y - rect.h/2),
            v2d(center.x + rect.w/2, center.y + rect.h/2),
            v2d(center.x - rect.w/2, center.y + rect.h/2)
    );
    RectVertices rotatedVertices;
    for(int i = 0; i < 4; i++){
        rotatedVertices[i] = v2d_rotate(initialVertices[i], v2d(center.x, center.y), rect.rotation);
    }
    return rotatedVertices;
}


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
    float m = (p2.y - p1.y)/(p2.x - p2.y);
    float b = p1.y - m * p1.x;
    float y = m * x + b;
    return y;
}

bool ray2dIntersectLineSegment(Ray2d ray, Position p1, Position p2, float &distanceFromRayOrigin){
    if(!isInRange(p1.x, p2.x, ray.startingPosition.x)){
        return false;
    }
    float y = getYForXOnLine(p1, p2, ray.startingPosition.x);
    distanceFromRayOrigin = ray.startingPosition.y - y;
    if(distanceFromRayOrigin > ray.distance || distanceFromRayOrigin < 0){
        return false;
    }
    return true;
}


void ray2dSolidRectCollisionSystem(flecs::iter &it, Position *positions, std::vector<Ray2d> *ray2dCollections, Velocity *velocities, State *states ){
    for(u64 i : it){
        // check against rectangular objects
        // if(velocities[i].y <= 0){
        //     continue;
        // }
        
        State state = STATE_IN_AIR;
        for(int vi = 0; vi < ray2dCollections[i].size(); vi++){
            if(velocities[i].y < 0){
                ray2dCollections[i][vi].distance = 18;
            }
            Ray2d rayLocal = ray2dCollections[i].at(vi);
            Ray2d rayGlobal;
            rayGlobal.startingPosition.x = positions[i].x + rayLocal.startingPosition.x;
            rayGlobal.startingPosition.y = positions[i].y + rayLocal.startingPosition.y;



            
            auto f = it.world().filter<Position, SolidRect>();
            f.each([&](flecs::entity e, Position &position, SolidRect &rectObj){
                SDL_Rect rect = {
                        (int)position.x - (int)rectObj.w/2,
                        (int)position.y - (int)rectObj.h/2,
                        (int)rectObj.w,
                        (int)rectObj.h
                };

                if(rayGlobal.startingPosition.x > rect.x
                && rayGlobal.startingPosition.x < rect.x + rect.w){
                    //ray x is in the rect
                    if(rayGlobal.startingPosition.y > rect.y
                    && rayGlobal.startingPosition.y < rect.y + rect.h){
                        ray2dCollections[i][0].distance = 32;
                        ray2dCollections[i][1].distance = 32;

                        
                        // ray y is inside rect
                        positions[i].y = rect.y - rayLocal.distance/2 ;
                        state = STATE_ON_GROUND;
                    }
                    else if(rayGlobal.startingPosition.y + ray2dCollections[i][vi].distance > rect.y
                    && rayGlobal.startingPosition.y + rayLocal.distance < rect.y + rect.h){
                        ray2dCollections[i][0].distance = 32;
                        ray2dCollections[i][1].distance = 32;


                        positions[i].y = rect.y - ray2dCollections[i][vi].distance/2 ;
                        state = STATE_ON_GROUND;
                    }
                }
                
            });
        }
        states[i] = state;
        if(state == STATE_IN_AIR){
            continue;
        }
        
    }
}