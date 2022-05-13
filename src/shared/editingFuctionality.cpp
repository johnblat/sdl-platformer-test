#include "position.h"
#include "flecs.h"
#include <SDL2/SDL.h>
#include "mouseState.h"


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


void EditPlatformVerticesAddVertexAtMousePositionOnSelected(flecs::world &ecs, MouseState &mouseState){
    if(mouseState.lmbCurrentState == INPUT_IS_JUST_RELEASED){
        Position p = {mouseState.cameraAdjustedPosition.x, mouseState.cameraAdjustedPosition.y};
        Position pv;
        pv.x = p.x;
        pv.y = p.y;

        // THIS IS WACKY.
        // TODO FIX ME
        bool NoneSelected = true;
        auto f = ecs.filter<PlatformVertices, SelectedForEditing>();
        f.iter([&](flecs::iter &it, PlatformVertices *pvs, SelectedForEditing *selected){
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