#include "collisions.h"
#include "flecs.h"
#include <SDL2/SDL.h>
#include "camera.h"
#include "render.h"
#include "ray2d.h"
#include "solid_rect.h"
#include "ints.h"
#include "window.h"
#include "shapes.h"



void renderPlatformVerticesSystem(flecs::iter &it, Position *positions, PlatformVertexCollection *platformVertexCollections){
    for(auto i : it){
        renderPolyLineInCamera(
            positions[i], 
            platformVertexCollections[i].vals, 
            platformVertexCollections[i].color
        );
    }
}


void renderPlatformVerticesNodesSystem(flecs::iter &it, Position *positions, PlatformVertexCollection *PlatformVerticesCollection){
    for(auto i : it){
        int size = PlatformVerticesCollection[i].vals.size();
        for(int j = 0; j < size; j++){
            Position vertexPositionGlobal;
            vertexPositionGlobal.x = positions[i].x + PlatformVerticesCollection[i].vals[j].x;
            vertexPositionGlobal.y = positions[i].y + PlatformVerticesCollection[i].vals[j].y;

            renderDiamondInCamera(vertexPositionGlobal, (SDL_Color){0,255,255,255});
        }
    }
}




void renderSensorsSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections){
    // move this somewhere else
    Position centerScreen = {(float)gScreenWidth/2.0f, (float)gScreenHeight/2.0f}; 

    for(u64 i : it){

        {
            float scale;
            SDL_RenderGetScale(gRenderer,&scale, nullptr );
            Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};

            Position actualPosition;
            actualPosition.x = positions[i].x + sensorCollections[i].rays[LF_SENSOR].startingPosition.x;
            actualPosition.y = positions[i].y + sensorCollections[i].rays[LF_SENSOR].startingPosition.y;
            actualPosition.x = actualPosition.x - gCameraPosition.x + scaledCenterScreen.x;
            actualPosition.y = actualPosition.y - gCameraPosition.y + scaledCenterScreen.y;


            SDL_SetRenderDrawColor(gRenderer,  167, 236, 29 ,255);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x, actualPosition.y , actualPosition.x, actualPosition.y  + sensorCollections[i].rays[LF_SENSOR].distance);

            SDL_SetRenderDrawColor(gRenderer, 0,0,0,255);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x - 1, actualPosition.y , actualPosition.x - 1, actualPosition.y  + sensorCollections[i].rays[LF_SENSOR].distance);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x + 1, actualPosition.y , actualPosition.x + 1, actualPosition.y  + sensorCollections[i].rays[LF_SENSOR].distance);


        }

        {
            float scale;
            SDL_RenderGetScale(gRenderer,&scale, nullptr );
            Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};

            Position actualPosition;
            actualPosition.x = positions[i].x + sensorCollections[i].rays[RF_SENSOR].startingPosition.x;
            actualPosition.y = positions[i].y + sensorCollections[i].rays[RF_SENSOR].startingPosition.y;
            actualPosition.x = actualPosition.x - gCameraPosition.x + scaledCenterScreen.x;
            actualPosition.y = actualPosition.y - gCameraPosition.y + scaledCenterScreen.y;


            SDL_SetRenderDrawColor(gRenderer, 29, 236, 158,255);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x, actualPosition.y , actualPosition.x, actualPosition.y  + sensorCollections[i].rays[RF_SENSOR].distance);

            SDL_SetRenderDrawColor(gRenderer, 0,0,0,255);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x - 1, actualPosition.y , actualPosition.x - 1, actualPosition.y  + sensorCollections[i].rays[RF_SENSOR].distance);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x + 1, actualPosition.y , actualPosition.x + 1, actualPosition.y  + sensorCollections[i].rays[RF_SENSOR].distance);


        }


        {
            float scale;
            SDL_RenderGetScale(gRenderer,&scale, nullptr );
            Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};

            Position actualPosition;
            actualPosition.x = positions[i].x + sensorCollections[i].rays[LW_SENSOR].startingPosition.x;
            actualPosition.y = positions[i].y + sensorCollections[i].rays[LW_SENSOR].startingPosition.y;
            actualPosition.x = actualPosition.x - gCameraPosition.x + scaledCenterScreen.x;
            actualPosition.y = actualPosition.y - gCameraPosition.y + scaledCenterScreen.y;


            SDL_SetRenderDrawColor(gRenderer,  255, 151, 241 ,255);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x, actualPosition.y , actualPosition.x - sensorCollections[i].rays[LW_SENSOR].distance, actualPosition.y  );

            SDL_SetRenderDrawColor(gRenderer, 0,0,0,255);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x , actualPosition.y+1 , actualPosition.x  - sensorCollections[i].rays[LW_SENSOR].distance, actualPosition.y+1  );
            SDL_RenderDrawLineF(gRenderer, actualPosition.x , actualPosition.y-1 , actualPosition.x  - sensorCollections[i].rays[LW_SENSOR].distance, actualPosition.y -1 );
        }

        {
            float scale;
            SDL_RenderGetScale(gRenderer,&scale, nullptr );
            Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};

            Position actualPosition;
            actualPosition.x = positions[i].x + sensorCollections[i].rays[RW_SENSOR].startingPosition.x;
            actualPosition.y = positions[i].y + sensorCollections[i].rays[RW_SENSOR].startingPosition.y;
            actualPosition.x = actualPosition.x - gCameraPosition.x + scaledCenterScreen.x;
            actualPosition.y = actualPosition.y - gCameraPosition.y + scaledCenterScreen.y;


            SDL_SetRenderDrawColor(gRenderer,  244, 0, 50 ,255);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x, actualPosition.y , actualPosition.x + sensorCollections[i].rays[RW_SENSOR].distance, actualPosition.y  );

            SDL_SetRenderDrawColor(gRenderer, 0,0,0,255);
            SDL_RenderDrawLineF(gRenderer, actualPosition.x , actualPosition.y+1 , actualPosition.x  + sensorCollections[i].rays[RW_SENSOR].distance, actualPosition.y+1  );
            SDL_RenderDrawLineF(gRenderer, actualPosition.x , actualPosition.y-1 , actualPosition.x  + sensorCollections[i].rays[RW_SENSOR].distance, actualPosition.y -1 );
        }
            // SDL_SetRenderDrawColor(gRenderer, 0,0,0,255);
            // SDL_RenderDrawLineF(gRenderer, actualPosition.x - 1, actualPosition.y , actualPosition.x - 1 + sensorCollections[i].rays[j].distance, actualPosition.y  );
            // SDL_RenderDrawLineF(gRenderer, actualPosition.x + 1, actualPosition.y , actualPosition.x + 1 + sensorCollections[i].rays[j].distance, actualPosition.y  );


        
    }
}
