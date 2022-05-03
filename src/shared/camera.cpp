#include "position.h"
#include "window.h"
#include "flecs.h"
#include "ints.h"
#include "input.h"


Position gCameraPosition;
Position gCenterScreen;
float gZoomAmount = 2.0f;
float gZoomSpeed = 0.005;

void inputZoomSystem(flecs::iter &it, Input *inputs){
    for(i64 i : it){
        if(inputIsPressed(inputs[i], "zoom-in")){
            gZoomAmount += 0.005;
        }
        if(inputIsPressed(inputs[i], "zoom-out")){
            gZoomAmount -= 0.005;
        }
        if(inputIsPressed(inputs[i], "zoom-reset")){
            gZoomAmount = 2.0;
        }
    }
}