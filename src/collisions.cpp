#include "collisions.h"
#include "states.h"
#include "position.h"
#include "flecs.h"
#include "velocity.h"
#include <vector>
#include "ray2d.h"
#include "solid_rect.h"

void ray2dRectangularObjectCollisionSystem(flecs::iter &it, Position *positions, std::vector<Ray2d> *ray2dCollections, Velocity *velocities, State *states ){
    for(int i : it){
        // check against rectangular objects
        // if(velocities[i].y <= 0){
        //     continue;
        // }
        
        State state = STATE_IN_AIR;
        Ray2d winRay;
        bool didCollide = false;
        for(int vi = 0; vi < ray2dCollections[i].size(); vi++){
            if(velocities[i].y < 0){
                ray2dCollections[i][vi].distance = 18;
            }
            Ray2d rayLocal = ray2dCollections[i].at(vi);
            Ray2d rayGlobal;
            rayGlobal.startingPosition.x = positions[i].x + rayLocal.startingPosition.x;
            rayGlobal.startingPosition.y = positions[i].y + rayLocal.startingPosition.y;

            

            
            auto f = it.world().filter<SolidRect>();
            f.each([&](flecs::entity e, SolidRect &rectObj){
                if(rayGlobal.startingPosition.x > rectObj.rect.x
                && rayGlobal.startingPosition.x < rectObj.rect.x + rectObj.rect.w){
                    //ray x is in the rect
                    if(rayGlobal.startingPosition.y > rectObj.rect.y
                    && rayGlobal.startingPosition.y < rectObj.rect.y + rectObj.rect.h){
                        ray2dCollections[i][0].distance = 32;
                        ray2dCollections[i][1].distance = 32;

                        
                        // ray y is inside rect
                        positions[i].y = rectObj.rect.y - rayLocal.distance/2 ;
                        state = STATE_ON_GROUND;
                    }
                    else if(rayGlobal.startingPosition.y + ray2dCollections[i][vi].distance > rectObj.rect.y
                    && rayGlobal.startingPosition.y + rayLocal.distance < rectObj.rect.y + rectObj.rect.h){
                        ray2dCollections[i][0].distance = 32;
                        ray2dCollections[i][1].distance = 32;


                        positions[i].y = rectObj.rect.y - ray2dCollections[i][vi].distance/2 ;
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