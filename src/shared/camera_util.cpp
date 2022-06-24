#include "camera.h"
#include "position.h"
#include "render.h"
#include "v2d.h"
#include "window.h"


v2d 
cam_util_world_position_to_camera_position(v2d w){
    Position centerScreen = {(float)gScreenWidth/2.0f, (float)gScreenHeight/2.0f}; 
    
    float scale;
    
    SDL_RenderGetScale(gRenderer, &scale, nullptr);
    
    Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};
    
    v2d c = {
        w.x - gCameraPosition.x + scaledCenterScreen.x, 
        w.y - gCameraPosition.y + scaledCenterScreen.y
    };
    
    return c;
}