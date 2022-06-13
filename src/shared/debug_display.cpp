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
#include "mouseState.h"



void setColorOnPVCSelect(flecs::iter &it, PlatformVertexCollection *pvcs){
    for(u32 i : it){
        SDL_Color selectedColor = {20,250,20,255};
        pvcs[i].edgeColor = selectedColor;
        pvcs[i].nodeColor = (SDL_Color){0,200,0,255};
    }
}

void setColorOnPVCDeselect(flecs::iter &it, PlatformVertexCollection *pvcs){
    for(u32 i : it){
        SDL_Color defaultColor = {255,255,255,255};
        pvcs[i].edgeColor = defaultColor;
        pvcs[i].nodeColor = (SDL_Color){0,255,255,255};
    }
}

void renderPlatformVerticesSystem(flecs::iter &it, Position *positions, PlatformVertexCollection *platformVertexCollections){
    for(auto i : it){
        renderPolyLineInCamera(
            positions[i], 
            platformVertexCollections[i].vals, 
            platformVertexCollections[i].edgeColor
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

            renderDiamondInCamera(vertexPositionGlobal, PlatformVerticesCollection[i].nodeColor);
        }
    }
}

v2d rotateSensorPositionBasedOnGroundMode(v2d v, v2d o, GroundMode groundMode){
    v2d rotated = v;
    if(groundMode == LEFT_WALL_GM){
        rotated = v2dRotate90DegreesCW(rotated, o);
    }
    else if(groundMode == RIGHT_WALL_GM){
        rotated = v2dRotate90DegreesCCW(rotated, o);
    }
    else if(groundMode == CEILING_GM ){
        rotated = v2dRotate180Degrees(v, o);
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
            else if(groundModes[i] == CEILING_GM){
                renderBorderedVerticalLine(positionInCamera.y, positionInCamera.y - sensorCollections[i].rays[LF_SENSOR].distance, positionInCamera.x, fillColor, borderColor);
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
            else if(groundModes[i] == CEILING_GM) {
                renderBorderedVerticalLine(positionInCamera.y, positionInCamera.y - sensorCollections[i].rays[RF_SENSOR].distance, positionInCamera.x, fillColor, borderColor);
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
            else if(groundModes[i] == CEILING_GM) {
                renderBorderedHorizontalLine(positionInCamera.x, positionInCamera.x + sensorCollections[i].rays[LW_SENSOR].distance, positionInCamera.y, fillColor, borderColor);
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
            else if (groundModes[i] == CEILING_GM) {
                renderBorderedHorizontalLine(positionInCamera.x, positionInCamera.x + sensorCollections[i].rays[LW_SENSOR].distance, positionInCamera.y, fillColor, borderColor);
            }
            else {
                renderBorderedHorizontalLine(positionInCamera.x, positionInCamera.x + sensorCollections[i].rays[LW_SENSOR].distance, positionInCamera.y, fillColor, borderColor);
            }

        }
    }
}


void renderUncommitedLinesToPlaceSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates){
    for(int i : it){

        if(mouseStates[i].lmbCurrentState == INPUT_IS_PRESSED){

            Position mousePosition = {mouseStates[i].worldPosition.x, mouseStates[i].worldPosition.y};

            flecs::world ecs = it.world();

            Position tailVertex;
            bool NoneSelected = true;
            auto f = ecs.filter<Position, PlatformVertexCollection, SelectedForEditing>();
            f.iter([&](flecs::iter &it, Position *ps, PlatformVertexCollection *pvc, SelectedForEditing *selected){
                tailVertex = pvc[0].vals[pvc[0].vals.size()-1];
                Position localPosition = v2d_sub(mousePosition, ps[0]);

                if(inputIsPressed(inputs[i], "edit-angle-snap")){
                    
                    Position a = v2d_sub(localPosition, tailVertex);
                    if(inputIsPressed(inputs[i], "edit-angle-snap")){
                        if(abs(a.x) < abs(a.y)){
                            localPosition.x = pvc[0].vals.at(pvc[0].vals.size()-1).x;
                        }
                        else {
                            localPosition.y = pvc[0].vals.at(pvc[0].vals.size()-1).y;
                        }
                    }
                }
                

                
                NoneSelected = false;

                Position mouseAdjustedPos = v2d_add(localPosition, ps[0]);
                Position tailVertexAdjustedPos = v2d_add(tailVertex, ps[0]);

                if(NoneSelected){
                    SDL_SetRenderDrawColor(gRenderer, 255,150,255,255);
                    renderDiamondInCamera(mouseAdjustedPos, (SDL_Color){255,150,255,255});
                }
                else{
                    renderLineInCamera(tailVertexAdjustedPos, mouseAdjustedPos, (SDL_Color){255,150,255,255});
                    SDL_SetRenderDrawColor(gRenderer, 255,150,255,255);
                    renderDiamondInCamera(mouseAdjustedPos, (SDL_Color){255,150,255,255});

                }
            });

            

            
        }
    }
}