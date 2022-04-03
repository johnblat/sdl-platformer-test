#include "flecs.h"
#include "velocity.h"
#include "position.h"
#include "ints.h"
#include "input.h"
#include "util.h"
#include <vector>
#include "ray2d.h"
#include "states.h"
#include "stateProcessing.h"
#include <cmath>
#include "solid_rect.h"
#include "shapeTransformations.h"
#include "collisions.h"

void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions){
    

    for(auto i : it){
        positions[i].x += velocities[i].x ;//* it.delta_time();
        positions[i].y += velocities[i].y ;//* it.delta_time();
    }
}





static float groundSpeed = 0.0f;

void InputVelocitySetterSystem(flecs::iter &it, Velocity *velocities, Input *inputs, StateCurrPrev *states, Angle *angles){

    const float acc = 0.046875;// * 600;
    const float frc = 0.046875;// * 600;
    const float dec = 0.5;// * 600;
    const float topSpeed = 6;// * 600;
    const float jump = 6.5f;
    const float slp = 0.125f;
    const float slpRollUp = 0.078125f;
    const float slpRollDown = 0.3125f;
    const float fall = 2.5f;

    const float airAcc = 0.09375f;
    
   

    for(auto i : it){
        if(stateJustEntered(states[i], STATE_ON_GROUND)){

            // shallow
            if((angles[i].rads >= degToRads(0.0f) && angles[i].rads <= degToRads(23.0f))
            || (angles[i].rads >= degToRads(339.0f) && angles[i].rads <= degToRads(360.0f))){
                groundSpeed = velocities[i].x;
            }
            // half steep
            else if((angles[i].rads > degToRads(23.0f) && angles[i].rads <= degToRads(45.0f))
            || angles[i].rads >= degToRads(316.0f) && angles[i].rads < degToRads(339.0f)){
                float xSpeedAbs = fabs(velocities[i].x);
                float ySpeedAbs = fabs(velocities[i].y);
                if(xSpeedAbs > ySpeedAbs){
                    groundSpeed = velocities[i].x;
                }
                else{
                    groundSpeed = velocities[i].y * 0.5f * -sign(sin(angles[i].rads));
                }

            }
            // full steep
            else if((angles[i].rads > degToRads(46.0f) && angles[i].rads <= degToRads(90.0f))
            || (angles[i].rads >= degToRads(271.0f) && angles[i].rads < degToRads(316.0f))){
                float xSpeedAbs = fabs(velocities[i].x);
                float ySpeedAbs = fabs(velocities[i].y);
                if(xSpeedAbs > ySpeedAbs){
                    groundSpeed = velocities[i].x;
                }
                else{
                    groundSpeed = velocities[i].y * -sign(sin(angles[i].rads));
                }
            }
        }

        if(states[i].currentState == STATE_ON_GROUND){
            groundSpeed -= slp * sin(angles[i].rads);

            // side
            if(inputIsPressed(inputs[i], "left")){
                if(groundSpeed > 0){ // moving right
                    groundSpeed -= dec;
                }
                else { // moving left
                    groundSpeed -= acc;
                    groundSpeed = MAX(groundSpeed, -topSpeed);
                }
            }
            else if(inputIsPressed(inputs[i], "right")){
                if(groundSpeed < 0){ // moving left
                    groundSpeed += dec;
                }
                else { //moving right
                    groundSpeed += acc;
                    groundSpeed = MIN(groundSpeed, topSpeed);
                }
            }
            else {
                if(groundSpeed == 0){

                }
                else if(groundSpeed > 0){
                    groundSpeed -= frc;
                    if(groundSpeed < 0){
                        groundSpeed = 0;
                    }
                }
                else if(groundSpeed < 0){
                    groundSpeed += frc;
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


            if(inputIsJustPressed(inputs[i], "jump") && states[i].currentState != STATE_IN_AIR){
                setState(states[i], STATE_IN_AIR);
                velocities[i].x -= jump * sin(angles[i].rads);
                velocities[i].y -= jump * cos(angles[i].rads);
            }
        }
        else if(states[i].currentState == STATE_IN_AIR){
            if(!inputIsPressed(inputs[i], "jump")){
                if(velocities[i].y < -4.0f){
                    velocities[i].y = -4.0f;

                }
            }
            // side
            if(inputIsPressed(inputs[i], "left")){
                velocities[i].x -= airAcc;
                velocities[i].x = MAX(velocities[i].x, -topSpeed);
            }
            else if(inputIsPressed(inputs[i], "right")){
                velocities[i].x += airAcc;
                velocities[i].x = MIN(velocities[i].x, topSpeed);
            }
        }
       
    }
}


void gravitySystem(flecs::iter &it, Velocity *velocities, StateCurrPrev *states){
    const float grv = 0.21875f;
    for(int i : it){
        if(states[i].currentState == STATE_ON_GROUND){
            velocities[i].y = 0;
            continue;
        }
        if(states[i].currentState == STATE_IN_AIR){
            if(velocities[i].y < 0.0f && velocities[i].y > -4.0f){
                velocities[i].x -= ((velocities[i].x / 0.125f) / 256);
            }

            velocities[i].y += grv;
            if (velocities[i].y > 16.0f){
                velocities[i].y = 16.0f;
            } 
        }
    }
}
