#include "flecs.h"
#include "velocity.h"
#include "position.h"
#include "jbInts.h"
#include "input.h"

void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions){
    for(auto i : it){
        positions[i].x += velocities[i].x * it.delta_time();
        positions[i].y += velocities[i].y * it.delta_time();
    }
}


void keyStateVelocitySetterSystem(flecs::iter &it, Velocity *velocities, KeyboardState *keyStatesCollections){
    for(auto i : it){
        u8 *keyStates = keyStatesCollections[i].keyStates;
        // side
        if(keyStates[SDL_SCANCODE_A]){
            velocities[i].x = -100;
        }
        else if(keyStates[SDL_SCANCODE_D]){
            velocities[i].x = 100;
        }
        else {
            velocities[i].x = 0;
        }

    }
}