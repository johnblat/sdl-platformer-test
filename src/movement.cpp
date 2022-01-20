#include "flecs.h"
#include "velocity.h"
#include "position.h"
#include "jbInts.h"
#include "input.h"
#include "util.h"


void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions){
    

    for(auto i : it){
        positions[i].x += velocities[i].x;
        positions[i].y += velocities[i].y;
    }
}


void keyStateVelocitySetterSystem(flecs::iter &it, Velocity *velocities, KeyboardState *keyStatesCollections){
    const float acc = 0.046875 * 10;
    const float frc = 0.046875 * 10;
    const float topSpeed = 6 * 10;
    const float groundSpeed = 0;

    for(auto i : it){
        u8 *keyStates = keyStatesCollections[i].keyStates;
        // side
        if(keyStates[SDL_SCANCODE_A]){
            velocities[i].x -= acc * it.delta_time();
            velocities[i].x = MAX(velocities[i].x, -topSpeed);
        }
        else if(keyStates[SDL_SCANCODE_D]){
            velocities[i].x += acc * it.delta_time();
            velocities[i].x = MIN(velocities[i].x, topSpeed);
        }
        else {
            if(velocities[i].x == 0){

            }
            else if(velocities[i].x > 0){
                velocities[i].x -= frc * 20 * it.delta_time();
                if(velocities[i].x < 0){
                    velocities[i].x = 0;
                }
            }
            else if(velocities[i].x < 0){
                velocities[i].x += frc * 20 * it.delta_time();
                if(velocities[i].x > 0){
                    velocities[i].x = 0;
                }
            }
            else {
                velocities[i].x = 0;
            }
        }

    }
}