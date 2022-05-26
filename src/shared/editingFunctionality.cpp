#include "position.h"
#include "flecs.h"
#include <SDL2/SDL.h>
#include "mouseState.h"
#include "input.h"
#include "window.h"
#include "camera.h"
#include "render.h"

void EndEditingSelectedPlatformVertices(flecs::world &ecs){
    auto f = ecs.filter<PlatformVertexCollection, SelectedForEditing>();
    ecs.defer_begin();

    f.each([&](flecs::entity e, PlatformVertexCollection pvc, SelectedForEditing selected){
        e.remove<SelectedForEditing>();
    });

    ecs.defer_begin();
}


void createAndSelectPlatformVerticesEntity(flecs::world &ecs, PlatformVertexCollection pvc){

   // ecs.defer_begin();

    flecs::entity pvcEntity = ecs.entity();

    pvcEntity.set<Position>((Position){0,0});
    pvcEntity.set<PlatformVertexCollection>(pvc);
    pvcEntity.add<SelectedForEditing>();

   // ecs.defer_end();
}


void EditPlatformVerticesAddVertexAtMousePositionOnSelectedSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates){
    for(int i : it){

        if(mouseStates[i].lmbCurrentState == INPUT_IS_JUST_RELEASED){
            Position pv = {mouseStates[i].cameraAdjustedPosition.x, mouseStates[i].cameraAdjustedPosition.y};


            flecs::world ecs = it.world();
        
            bool NoneSelected = true;
            auto f = ecs.filter<PlatformVertexCollection, SelectedForEditing>();
            f.iter([&](flecs::iter &it, PlatformVertexCollection *pvc, SelectedForEditing *selected){
                
                Position tailVertex = pvc[i].vals.at(pvc[i].vals.size()-1);
                Position a = v2d_sub(pv, tailVertex);
                //Position a = pv - tailVertex;
                if(inputIsPressed(inputs[i], "edit-angle-snap")){
                    if(abs(a.x) < abs(a.y)){
                        pv.x = pvc[i].vals.at(pvc[i].vals.size()-1).x;
                    }
                    else {
                        pv.y = pvc[i].vals.at(pvc[i].vals.size()-1).y;
                    }
                }
                for(int i : it){
                    pvc[i].vals.push_back(pv);
                }
                NoneSelected = false;
            });

            if(NoneSelected){
                PlatformVertexCollection pvc;
                pvc.edgeColor = (SDL_Color){255,255,255,255};
                pvc.nodeColor = (SDL_Color){0,255,255,255};
                pvc.vals.push_back((pv));
                createAndSelectPlatformVerticesEntity(ecs, pvc);
            }
        }
    }
    
}


void renderUncommitedLinesToPlaceSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates){
    for(int i : it){

        if(mouseStates[i].lmbCurrentState == INPUT_IS_PRESSED){

            Position pv = {mouseStates[i].cameraAdjustedPosition.x, mouseStates[i].cameraAdjustedPosition.y};

            flecs::world ecs = it.world();

            Position tailVertex;
            bool NoneSelected = true;
            auto f = ecs.filter<PlatformVertexCollection, SelectedForEditing>();
            f.iter([&](flecs::iter &it, PlatformVertexCollection *pvc, SelectedForEditing *selected){
                tailVertex = pvc[i].vals[pvc[i].vals.size()-1];
                if(inputIsPressed(inputs[i], "edit-angle-snap")){
                    
                    Position a = v2d_sub(pv, tailVertex);
                    if(inputIsPressed(inputs[i], "edit-angle-snap")){
                        if(abs(a.x) < abs(a.y)){
                            pv.x = pvc[i].vals.at(pvc[i].vals.size()-1).x;
                        }
                        else {
                            pv.y = pvc[i].vals.at(pvc[i].vals.size()-1).y;
                        }
                    }
                }
                

                
                NoneSelected = false;
            });

            if(NoneSelected){
                SDL_SetRenderDrawColor(gRenderer, 255,150,255,255);
                renderDiamondInCamera(pv, (SDL_Color){255,150,255,255});
            }
            else{
                renderLineInCamera(tailVertex, pv, (SDL_Color){255,150,255,255});
                SDL_SetRenderDrawColor(gRenderer, 255,150,255,255);
                renderDiamondInCamera(pv, (SDL_Color){255,150,255,255});

            }

            
        }
    }
}



void DeselectInputSystem(flecs::iter &it, Input *inputs){
    
    for(int i : it){
        
        if(inputIsJustReleased(inputs[i], "deselect")){
            it.world().defer_begin();

            auto f = it.world().filter<SelectedForEditing>();
            f.each([&](flecs::entity e, SelectedForEditing s){
                e.remove<SelectedForEditing>();
            });


            it.world().defer_end();
        }
        
    }
}