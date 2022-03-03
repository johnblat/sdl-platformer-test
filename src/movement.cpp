#include "flecs.h"
#include "velocity.h"
#include "position.h"
#include "ints.h"
#include "input.h"
#include "util.h"
#include <vector>
#include "ray2d.h"
#include "states.h"
#include <cmath>

void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions){
    

    for(auto i : it){
        positions[i].x += velocities[i].x ;//* it.delta_time();
        positions[i].y += velocities[i].y ;//* it.delta_time();
    }
}

static float groundSpeed = 0.0f;

void InputVelocitySetterSystem(flecs::iter &it, Velocity *velocities, Input *inputs, State *states, Angle *angles){

    const float acc = 0.046875;// * 600;
    const float frc = 0.046875;// * 600;
    const float dec = 0.5;// * 600;
    const float topSpeed = 6;// * 600;
    const float jump = 6.5f;
    const float slp = 0.125f;
    
   

    for(auto i : it){
        if(states[i] == STATE_ON_GROUND){
            groundSpeed -= slp * sin(angles[i].rads);

            // side
            if(inputIsPressed(inputs[i], "left")){
                if(groundSpeed > 0){ // moving right
                    groundSpeed -= dec;// * it.delta_time();
                }
                else { // moving left
                    groundSpeed -= acc;// * it.delta_time();
                    groundSpeed = MAX(groundSpeed, -topSpeed);
                }
            }
            else if(inputIsPressed(inputs[i], "right")){
                if(groundSpeed < 0){ // moving left
                    groundSpeed += dec;// * it.delta_time();
                }
                else { //moving right
                    groundSpeed += acc;// * it.delta_time();
                    groundSpeed = MIN(groundSpeed, topSpeed);
                }
            }
            else {
                if(groundSpeed == 0){

                }
                else if(groundSpeed > 0){
                    groundSpeed -= frc;//  * it.delta_time();
                    if(groundSpeed < 0){
                        groundSpeed = 0;
                    }
                }
                else if(groundSpeed < 0){
                    groundSpeed += frc;// * it.delta_time();
                    if(groundSpeed > 0){
                        groundSpeed = 0;
                    }
                }
                else {
                    groundSpeed = 0;
                }
            }

            velocities[i].x = groundSpeed * cos(angles[i].rads);
            velocities[i].y = groundSpeed * -sin(angles[i].rads);


            if(inputIsJustPressed(inputs[i], "jump") && states[i] != STATE_IN_AIR){
                states[i] = STATE_IN_AIR;
                velocities[i].y = -jump;
            }
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
