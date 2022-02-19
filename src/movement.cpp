#include "flecs.h"
#include "velocity.h"
#include "position.h"
#include "jbInts.h"
#include "input.h"
#include "util.h"
#include <vector>
#include "ray2d.h"
#include "states.h"

void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions){
    

    for(auto i : it){
        positions[i].x += velocities[i].x ;//* it.delta_time();
        positions[i].y += velocities[i].y ;//* it.delta_time();
    }
}


void InputVelocitySetterSystem(flecs::iter &it, Velocity *velocities, Input *inputs, State *states){

    const float acc = 0.046875;// * 600;
    const float frc = 0.046875;// * 600;
    const float dec = 0.5;// * 600;
    const float topSpeed = 6;// * 600;
    const float groundSpeed = 0;
    const float gravAcc = 0.046875;
    const float jump = 6.5f;

    for(auto i : it){
        // side
        if(inputIsPressed(inputs[i], "left")){
            if(velocities[i].x > 0){ // moving right
                velocities[i].x -= dec;// * it.delta_time();
            }
            else { // moving left
                velocities[i].x -= acc;// * it.delta_time();
                velocities[i].x = MAX(velocities[i].x, -topSpeed);
            }
        }
        else if(inputIsPressed(inputs[i], "right")){
            if(velocities[i].x < 0){ // moving left
                velocities[i].x += dec;// * it.delta_time();
            }
            else { //moving right
                velocities[i].x += acc;// * it.delta_time();
                velocities[i].x = MIN(velocities[i].x, topSpeed);
            }
        }
        else {
            if(velocities[i].x == 0){

            }
            else if(velocities[i].x > 0){
                velocities[i].x -= frc;//  * it.delta_time();
                if(velocities[i].x < 0){
                    velocities[i].x = 0;
                }
            }
            else if(velocities[i].x < 0){
                velocities[i].x += frc;// * it.delta_time();
                if(velocities[i].x > 0){
                    velocities[i].x = 0;
                }
            }
            else {
                velocities[i].x = 0;
            }
        }

        if(inputIsJustPressed(inputs[i], "jump")){
            states[i] = STATE_IN_AIR;
            velocities[i].y = -jump;
        }
    }
}


void gravitySystem(flecs::iter &it, Velocity *velocities, State *states){
    const float grv = 0.21875f;
    const float max = 6.0f;
    for(int i : it){
        if(states[i] == STATE_ON_GROUND){
            velocities[i].y = 0;
            continue;
        }
        if(velocities[i].y < max){
            velocities[i].y += grv;
        }
    }
}
