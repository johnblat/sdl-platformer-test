#include "flecs.h"
#include "velocity.h"
#include "position.h"
#include "jbInts.h"
#include "input.h"
#include "util.h"


void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions){
    

    for(auto i : it){
        positions[i].x += velocities[i].x ;//* it.delta_time();
        positions[i].y += velocities[i].y ;//* it.delta_time();
    }
}


void InputVelocitySetterSystem(flecs::iter &it, Velocity *velocities, Input *inputs){
    const float acc = 0.046875;// * 600;
    const float frc = 0.046875;// * 600;
    const float dec = 0.5;// * 600;
    const float topSpeed = 6;// * 600;
    const float groundSpeed = 0;

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

        // gravity
        velocities[i].y = 1.0f;

    }
}