#include "position.h"
#include "flecs.h"
#include <SDL2/SDL.h>
#include "mouseState.h"
#include "input.h"
#include "window.h"
#include "camera.h"
#include "render.h"

void EndEditingSelectedPlatformVertices(flecs::world &ecs){
    auto f = ecs.filter<PlatformVertices, SelectedForEditing>();
    ecs.defer_begin();

    f.each([&](flecs::entity e, PlatformVertices pvs, SelectedForEditing selected){
        e.remove<SelectedForEditing>();
    });

    ecs.defer_begin();
}


void createAndSelectPlatformVerticesEntity(flecs::world &ecs, PlatformVertices pvs){

   // ecs.defer_begin();

    flecs::entity pvsEntity = ecs.entity();

    pvsEntity.set<Position>((Position){0,0});
    pvsEntity.set<PlatformVertices>(pvs);
    pvsEntity.add<SelectedForEditing>();

   // ecs.defer_end();
}


void EditPlatformVerticesAddVertexAtMousePositionOnSelectedSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates){
    for(int i : it){

        if(mouseStates[i].lmbCurrentState == INPUT_IS_JUST_RELEASED){
            Position pv = {mouseStates[i].cameraAdjustedPosition.x, mouseStates[i].cameraAdjustedPosition.y};


            flecs::world ecs = it.world();
        
            bool NoneSelected = true;
            auto f = ecs.filter<PlatformVertices, SelectedForEditing>();
            f.iter([&](flecs::iter &it, PlatformVertices *pvs, SelectedForEditing *selected){
                if(inputIsPressed(inputs[i], "edit-angle-snap")){
                    pv.x = pvs[i].vals.at(pvs[i].vals.size()-1).x;
                }
                for(int i : it){
                    pvs[i].vals.push_back(pv);
                }
                NoneSelected = false;
            });

            if(NoneSelected){
                PlatformVertices pvs;
                pvs.color = (SDL_Color){255,255,255,255};
                pvs.vals.push_back((pv));
                createAndSelectPlatformVerticesEntity(ecs, pvs);
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
            auto f = ecs.filter<PlatformVertices, SelectedForEditing>();
            f.iter([&](flecs::iter &it, PlatformVertices *pvs, SelectedForEditing *selected){
                if(inputIsPressed(inputs[i], "edit-angle-snap")){
                    pv.x = pvs[i].vals.at(pvs[i].vals.size()-1).x;
                }
                tailVertex = pvs[i].vals[pvs[i].vals.size()-1];

                
                NoneSelected = false;
            });

            if(NoneSelected){
                SDL_SetRenderDrawColor(gRenderer, 255,0,255,255);
                renderDiamondInCamera(pv, (SDL_Color){255,0,255,255});
            }
            else{
                renderLineInCamera(tailVertex, pv, (SDL_Color){255,0,255,255});
                SDL_SetRenderDrawColor(gRenderer, 255,0,255,255);
                renderDiamondInCamera(pv, (SDL_Color){255,0,255,255});

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