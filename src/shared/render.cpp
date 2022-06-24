#include "render.h"
#include "camera.h"
#include "position.h"
#include <SDL2/SDL.h>
#include "window.h"
#include "timestep.h"


void render_frame_start_System(flecs::iter &it){
    SDL_Color bgColor = {20,20,20,255};
    SDL_SetRenderDrawColor(gRenderer, bgColor.r, bgColor.b, bgColor.g, 255);
    SDL_RenderClear(gRenderer);
}

void render_end_frame_System(flecs::iter &it){
    SDL_RenderPresent(gRenderer);
    ts_TimeStep_delay_remaining_time(gTimeStep);
}


void render_line(Position p1, Position p2, SDL_Color color){

    SDL_SetRenderDrawColor(
        gRenderer, 
        color.r,
        color.g,
        color.b,
        255
    ); 

    p1 = cam_util_world_position_to_camera_position(p1);
    p2 = cam_util_world_position_to_camera_position(p2);

    SDL_RenderDrawLineF(gRenderer, p1.x, p1.y, p2.x, p2.y);

}



void render_poly_line(Position offsetPosition, std::vector<Position> points, SDL_Color color){
   SDL_SetRenderDrawColor(
        gRenderer, 
        color.r,
        color.g,
        color.b,
        255
    ); 

    Position centerScreen = {(float)gScreenWidth/2.0f, (float)gScreenHeight/2.0f};


    float scale;
    SDL_RenderGetScale(gRenderer,&scale, nullptr );
    Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};

    std::vector<Position> cameraPlatformNode;
    for(int j = 0; j < points.size(); j++){
        Position pv;
        pv.x =  offsetPosition.x + points[j].x - gCameraPosition.x + scaledCenterScreen.x;
        pv.y = offsetPosition.y + points[j].y - gCameraPosition.y + scaledCenterScreen.y;

        cameraPlatformNode.push_back(pv);

    }
    if(cameraPlatformNode.size() < 1){
        return;
    }
    for(int i = 0; i < cameraPlatformNode.size() - 1; i++){
        v2d p1(cameraPlatformNode.at(i).x, cameraPlatformNode.at(i).y);
        v2d p2(cameraPlatformNode.at(i+1).x, cameraPlatformNode.at(i+1).y);
        SDL_RenderDrawLineF(gRenderer, p1.x, p1.y, p2.x, p2.y);
    }
}

void render_diamond(Position centerPoint, SDL_Color color){
    const int DIAMOND_RADIUS = 3;

    Position centerScreen = {(float)gScreenWidth/2.0f, (float)gScreenHeight/2.0f};

    float scale;
    SDL_RenderGetScale(gRenderer,&scale, nullptr );
    Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};

    SDL_SetRenderDrawColor(gRenderer, color.r, color.g, color.b, color.a);

    for(int y = -DIAMOND_RADIUS; y < DIAMOND_RADIUS+1; y++){
        float y1 = centerPoint.y + y   - gCameraPosition.y + scaledCenterScreen.y;
        float x1 = centerPoint.x - (DIAMOND_RADIUS - abs(y))   - gCameraPosition.x + scaledCenterScreen.x;

        float y2 = y1;
        float x2 = centerPoint.x + (DIAMOND_RADIUS - abs(y))   - gCameraPosition.x + scaledCenterScreen.x;

        SDL_RenderDrawLineF(gRenderer, x1, y1, x2, y2);
    }

}

