#include "position.h"
#include "window.h"
#include "flecs.h"
#include "ints.h"
#include "input.h"
#include "render.h"


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

void inputCameraMoveSystem(flecs::iter &it, Input *inputs){
    for(i64 i : it){
        if(inputIsPressed(inputs[i], "camera-pan-up")){
            gCameraPosition.y -= 3;
        }
        if(inputIsPressed(inputs[i], "camera-pan-down")){
            gCameraPosition.y += 3;
        }
        if(inputIsPressed(inputs[i], "camera-pan-left")){
            gCameraPosition.x -= 3;
        }
        if(inputIsPressed(inputs[i], "camera-pan-right")){
            gCameraPosition.x += 3;
        }
    }
}


void zoomRenderSetupSystem(flecs::iter &it){
    SDL_RenderSetScale(gRenderer, gZoomAmount, gZoomAmount);
}