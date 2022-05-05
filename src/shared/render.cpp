#include "render.h"
#include "camera.h"
#include "position.h"
#include <SDL2/SDL.h>
#include "window.h"


void renderPolyLineInCamera(Position offsetPosition, std::vector<Position> points, SDL_Color color){
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

    std::vector<Position> cameraPlatformVertices;
    for(int j = 0; j < points.size(); j++){
        Position pv;
        pv.x = offsetPosition.x + points[j].x - gCameraPosition.x + scaledCenterScreen.x;
        pv.y = offsetPosition.y + points[j].y - gCameraPosition.y + scaledCenterScreen.y;

        cameraPlatformVertices.push_back(pv);

    }
    if(cameraPlatformVertices.size() < 1){
        return;
    }
    for(int i = 0; i < cameraPlatformVertices.size() - 1; i++){
        v2d p1(cameraPlatformVertices.at(i).x, cameraPlatformVertices.at(i).y);
        v2d p2(cameraPlatformVertices.at(i+1).x, cameraPlatformVertices.at(i+1).y);
        SDL_RenderDrawLineF(gRenderer, p1.x, p1.y, p2.x, p2.y);
    }
}
