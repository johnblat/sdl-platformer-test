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
           if(Input_is_pressed(inputs[i], "zoom-in")){
            gZoomAmount += 0.005;
        }
        if(Input_is_pressed(inputs[i], "zoom-out")){
            gZoomAmount -= 0.005;
        }
        if(Input_is_pressed(inputs[i], "zoom-reset")){
            gZoomAmount = 2.0;
        }
    }
}

void inputCameraMoveSystem(flecs::iter &it, Input *inputs){
    for(i64 i : it){
        if(Input_is_pressed(inputs[i], "camera-pan-up")){
            gCameraPosition.y -= 3;
        }
        if(Input_is_pressed(inputs[i], "camera-pan-down")){
            gCameraPosition.y += 3;
        }
        if(Input_is_pressed(inputs[i], "camera-pan-left")){
            gCameraPosition.x -= 3;
        }
        if(Input_is_pressed(inputs[i], "camera-pan-right")){
            gCameraPosition.x += 3;
        }
    }
}


void zoomRenderSetupSystem(flecs::iter &it){
    SDL_RenderSetScale(gRenderer, gZoomAmount, gZoomAmount);
}

v2d worldPositionToCamPosition(v2d w){
    Position centerScreen = {(float)gScreenWidth/2.0f, (float)gScreenHeight/2.0f}; 
    float scale;
    SDL_RenderGetScale(gRenderer,&scale, nullptr );
    Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};
    v2d c = {
        w.x - gCameraPosition.x + scaledCenterScreen.x, 
        w.y - gCameraPosition.y + scaledCenterScreen.y
    };
    return c;
}