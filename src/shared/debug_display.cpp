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
#include "v2d.h"



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

v2d rotateSensorPositionBasedOnGroundMode(v2d v, v2d o, GroundMode groundMode){
    v2d rotated = v;
    if(groundMode == LEFT_WALL_GM){
        rotated = v2dRotate90DegreesCounterClockWise(rotated, o);
    }
    else if(groundMode == RIGHT_WALL_GM){
        rotated = v2dRotate90DegreesClockWise(rotated, o);
    }
    return rotated;
}


void renderBorderedHorizontalLine(float startX, float endX, float y, SDL_Color fillColor, SDL_Color borderColor){
    SDL_SetRenderDrawColor(gRenderer,  fillColor.r, fillColor.g, fillColor.b ,fillColor.a);
    SDL_RenderDrawLineF(gRenderer, startX, y , endX, y);

    SDL_SetRenderDrawColor(gRenderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawLineF(gRenderer, startX , y - 1 , endX, y - 1);
    SDL_RenderDrawLineF(gRenderer, startX, y  + 1, endX , y + 1);
}


void renderBorderedVerticalLine(float startY, float endY, float x, SDL_Color fillColor, SDL_Color borderColor){
    SDL_SetRenderDrawColor(gRenderer,  fillColor.r, fillColor.g, fillColor.b ,fillColor.a);
    SDL_RenderDrawLineF(gRenderer, x, startY , x, endY);

    SDL_SetRenderDrawColor(gRenderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    SDL_RenderDrawLineF(gRenderer, x - 1, startY , x - 1, endY);
    SDL_RenderDrawLineF(gRenderer, x + 1, startY , x + 1, endY);
}

v2d localToGlobal(v2d l, v2d o){
    v2d g = l + o;
    return g;
}


void renderSensorsSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections, GroundMode *groundModes){
    // move this somewhere else
    Position centerScreen = {(float)gScreenWidth/2.0f, (float)gScreenHeight/2.0f}; 

    for(u64 i : it){

        {
            v2d lfRotatedPos = rotateSensorPositionBasedOnGroundMode(sensorCollections[i].rays[LF_SENSOR].startingPosition, v2d(0.0f, 0.0f), groundModes[i]);

            v2d globalPosition = localToGlobal(lfRotatedPos, positions[i]);

            v2d positionInCamera = worldPositionToCamPosition(globalPosition);

            SDL_Color fillColor = {167, 236, 29 ,255};
            SDL_Color borderColor = {0,0,0,255};

            if(groundModes[i] == LEFT_WALL_GM){
                renderBorderedHorizontalLine(positionInCamera.x, positionInCamera.x - sensorCollections[i].rays[LF_SENSOR].distance, positionInCamera.y, fillColor, borderColor);
            }
            else if(groundModes[i] == RIGHT_WALL_GM){
                renderBorderedHorizontalLine(positionInCamera.x, positionInCamera.x + sensorCollections[i].rays[LF_SENSOR].distance, positionInCamera.y, fillColor, borderColor);
            }
            else {
                renderBorderedVerticalLine(positionInCamera.y, positionInCamera.y + sensorCollections[i].rays[LF_SENSOR].distance, positionInCamera.x, fillColor, borderColor);
            }
            
        }

        {
            v2d rfRotatedPos = rotateSensorPositionBasedOnGroundMode(sensorCollections[i].rays[RF_SENSOR].startingPosition, v2d(0.0f, 0.0f), groundModes[i]);

            v2d globalPosition = localToGlobal(rfRotatedPos, positions[i]);

            v2d positionInCamera = worldPositionToCamPosition(globalPosition);

            SDL_Color fillColor = {29, 236, 158,255};
            SDL_Color borderColor = {0,0,0,255};

            if(groundModes[i] == LEFT_WALL_GM){
                renderBorderedHorizontalLine(positionInCamera.x, positionInCamera.x - sensorCollections[i].rays[RF_SENSOR].distance, positionInCamera.y, fillColor, borderColor);
            }
            else if(groundModes[i] == RIGHT_WALL_GM){
                renderBorderedHorizontalLine(positionInCamera.x, positionInCamera.x + sensorCollections[i].rays[RF_SENSOR].distance, positionInCamera.y, fillColor, borderColor);
            }
            else {
                renderBorderedVerticalLine(positionInCamera.y, positionInCamera.y + sensorCollections[i].rays[RF_SENSOR].distance, positionInCamera.x, fillColor, borderColor);
            }
        }


        {
            v2d lwRotatedPos = rotateSensorPositionBasedOnGroundMode(sensorCollections[i].rays[LW_SENSOR].startingPosition, v2d(0.0f, 0.0f), groundModes[i]);

            v2d globalPosition = localToGlobal(lwRotatedPos, positions[i]);

            v2d positionInCamera = worldPositionToCamPosition(globalPosition);

            SDL_Color fillColor = {255, 151, 241 ,255};
            SDL_Color borderColor = {0,0,0,255};

            if(groundModes[i] == LEFT_WALL_GM){
                renderBorderedVerticalLine(positionInCamera.y, positionInCamera.y - sensorCollections[i].rays[LW_SENSOR].distance, positionInCamera.x, fillColor, borderColor);
            }
            else if(groundModes[i] == RIGHT_WALL_GM){
                renderBorderedVerticalLine(positionInCamera.y, positionInCamera.y + sensorCollections[i].rays[LW_SENSOR].distance, positionInCamera.x, fillColor, borderColor);
            }
            else {
                renderBorderedHorizontalLine(positionInCamera.x, positionInCamera.x - sensorCollections[i].rays[LW_SENSOR].distance, positionInCamera.y, fillColor, borderColor);
            }
        }

        {
            v2d lwRotatedPos = rotateSensorPositionBasedOnGroundMode(sensorCollections[i].rays[LW_SENSOR].startingPosition, v2d(0.0f, 0.0f), groundModes[i]);

            v2d globalPosition = localToGlobal(lwRotatedPos, positions[i]);

            v2d positionInCamera = worldPositionToCamPosition(globalPosition);

            SDL_Color fillColor = {244, 0, 50 ,255};
            SDL_Color borderColor = {0,0,0,255};

            if(groundModes[i] == LEFT_WALL_GM){
                renderBorderedVerticalLine(positionInCamera.y, positionInCamera.y + sensorCollections[i].rays[LW_SENSOR].distance, positionInCamera.x, fillColor, borderColor);
            }
            else if(groundModes[i] == RIGHT_WALL_GM){
                renderBorderedVerticalLine(positionInCamera.y, positionInCamera.y - sensorCollections[i].rays[LW_SENSOR].distance, positionInCamera.x, fillColor, borderColor);
            }
            else {
                renderBorderedHorizontalLine(positionInCamera.x, positionInCamera.x + sensorCollections[i].rays[LW_SENSOR].distance, positionInCamera.y, fillColor, borderColor);
            }

        }
    }
}
