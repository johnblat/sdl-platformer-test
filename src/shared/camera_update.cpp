#include "position.h"
#include "window.h"
#include "flecs.h"
#include "ints.h"
#include "input.h"
#include "render.h"


Position gCameraPosition;
Position gCenterScreen;
float gZoomAmount = 3.0f;
float gZoomSpeed = 0.005;


void 
cam_input_zoom_System(flecs::iter &it, Input *inputs){
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

void 
cam_input_camera_move_System(flecs::iter &it, Input *inputs){
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


void 
cam_zoom_render_frame_start_System(flecs::iter &it){
    SDL_RenderSetScale(gRenderer, gZoomAmount, gZoomAmount);
}

