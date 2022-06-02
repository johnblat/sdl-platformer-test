#include "position.h"
#include "flecs.h"
#include <SDL2/SDL.h>
#include "mouseState.h"
#include "input.h"
#include "window.h"
#include "camera.h"
#include "render.h"
#include "ray2d.h"
#include "util.h"

void deselectAll(flecs::world &ecs){
    ecs.defer_begin();

    auto f = ecs.filter<SelectedForEditing>();
    f.each([&](flecs::entity e, SelectedForEditing s){
        e.remove<SelectedForEditing>();
    });


    ecs.defer_end();
}


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

bool PointIntersectPointWithTolerance(v2d p1, v2d p2, float tolerance){
    v2d v = p1 - p2;
    float distance = v2d_magnitude(v);
    if(fabs(distance) <= tolerance){
        return true;
    }
    return false;
}

bool HorizontalRayIntersectLineSegment(Ray2d ray, v2d linePoint1, v2d linePoint2){
    if(!isInRange(linePoint1.y, linePoint2.y, ray.startingPosition.y)){
        return false;
    }
    float x = getXForYOnLine(linePoint1, linePoint2, ray.startingPosition.y);
    float distanceFromRayStartPoint = fabs(x - ray.startingPosition.x);
    if(distanceFromRayStartPoint > ray.distance || distanceFromRayStartPoint < 0){
        return false;
    }
    return true;
}


bool VerticalRayIntersectLineSegment(Ray2d ray, v2d linePoint1, v2d linePoint2){
    if(!isInRange(linePoint1.x, linePoint2.x, ray.startingPosition.x)){
        return false;
    }
    float y = getYForXOnLine(linePoint1, linePoint2, ray.startingPosition.x);
    float distanceFromRayStartPoint = fabs(y - ray.startingPosition.y);
    if(distanceFromRayStartPoint > ray.distance || distanceFromRayStartPoint < 0){
        return false;
    }
    return true;
}

bool PointIntersectLineWithTolerance(v2d linePoint1, v2d linePoint2, v2d p, float tolerance ){
    Ray2d ray;
    ray.startingPosition = p;
    ray.distance = tolerance;

    if(VerticalRayIntersectLineSegment(ray, linePoint1, linePoint2)){
        return true;
    }
    else if(HorizontalRayIntersectLineSegment(ray, linePoint1, linePoint2)){
        return true;
    }
    return false;
}



void EditPlatformVerticesAddVertexAtMousePositionOnSelectedSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates){
    for(int i : it){

        if(mouseStates[i].lmbCurrentState == INPUT_IS_JUST_RELEASED){
            Position mousePosition = {mouseStates[i].cameraAdjustedPosition.x, mouseStates[i].cameraAdjustedPosition.y};

            flecs::world ecs = it.world();

            bool selectedExistingPVC = false;
            auto f1 = ecs.filter<PlatformVertexCollection>();
            f1.iter([&](flecs::iter &it, PlatformVertexCollection *pvcs){
                float distanceForSelectionTolerance = 8.0f; // how far away can user click

                for(u32 j : it){
                    if(it.entity(j).has<SelectedForEditing>()){
                        continue;
                    }
                    for(int k = 0; k < pvcs[j].vals.size()- 1; k++){
                        v2d nodePosition = pvcs[j].vals[k];
                        v2d nodePosition2 = pvcs[j].vals[k+1];
                        // v2d v = nodePosition - mousePosition;
                        // float distance = v2d_magnitude(v);
                        if(PointIntersectLineWithTolerance(nodePosition, nodePosition2, mousePosition, distanceForSelectionTolerance)){
                            flecs::world world = it.world();
                            deselectAll(world);
                            it.world().defer_begin();
                            it.entity(j).add<SelectedForEditing>();
                            it.world().defer_end();
                            selectedExistingPVC = true;
                            break;
                        }
                    }

                }
            });

            if(selectedExistingPVC){
                break;
            }

            bool NoneSelected = true;
            auto f = ecs.filter<PlatformVertexCollection, SelectedForEditing>();
            f.iter([&](flecs::iter &it, PlatformVertexCollection *pvc, SelectedForEditing *selected){
                for(u32 j : it){
                    Position tailVertex = pvc[j].vals.at(pvc[j].vals.size()-1);
                    Position a = v2d_sub(mousePosition, tailVertex);

                    if(inputIsPressed(inputs[i], "edit-angle-snap")){
                        if(abs(a.x) < abs(a.y)){
                            mousePosition.x = pvc[j].vals.at(pvc[j].vals.size()-1).x;
                        }
                        else {
                            mousePosition.y = pvc[j].vals.at(pvc[j].vals.size()-1).y;
                        }
                    }
                    pvc[j].vals.push_back(mousePosition);
                    
                    NoneSelected = false;
                }
                
            });

            if(NoneSelected){
                PlatformVertexCollection pvc;
                pvc.edgeColor = (SDL_Color){255,255,255,255};
                pvc.nodeColor = (SDL_Color){0,255,255,255};
                pvc.vals.push_back((mousePosition));
                createAndSelectPlatformVerticesEntity(ecs, pvc);
            }
        }
    }
    
}

void SelectedPlatformVertexCollectionDeletionSystem(flecs::iter &it, Position *positions, PlatformVertexCollection *pvcs, SelectedForEditing *s){
    flecs::world world = it.world();
    auto f = world.filter<Input>();
    bool shouldDelete = false;
    f.each([&](flecs::entity e, Input input){
        if(inputIsJustReleased(input, "delete")){
            shouldDelete = true;
        }
    });
    if(shouldDelete){
        world.defer_begin();
        for(u32 i : it){
            it.entity(i).destruct();
        }
        world.defer_end();
    }

}

void SelectPlatformVertexOnMouseClick(flecs::iter &it, Position *positions, PlatformVertexCollection *pvcs, SelectedForEditing *ss){
    float distanceForSelectionTolerance = 5.0f;
    auto f = it.world().filter<MouseState>();

    f.each([&](flecs::entity e, MouseState ms){
        if(ms.lmbCurrentState == INPUT_IS_JUST_RELEASED){
            v2d mousePosition = ms.cameraAdjustedPosition;
            for(u32 i : it){
                for(int j = 0; j < pvcs[i].vals.size(); j++){
                    if(PointIntersectPointWithTolerance(mousePosition, pvcs[i].vals[j], distanceForSelectionTolerance)){
                        SelectedForEditingNode sn;
                        sn.idx = j;
                    }
                }
            }
        }
    });
}


// UNUSED
void SelectPlatformVertexCollectionOnMouseClick(flecs::iter &it, Position *positions, PlatformVertexCollection *pvcs){
    float distanceForSelectionTolerance = 5.0f; // how far away can user click
    auto f = it.world().filter<MouseState>();
    f.each([&](flecs::entity e, MouseState ms){
        if(ms.lmbCurrentState == INPUT_IS_JUST_RELEASED){
            v2d mousePosition = ms.cameraAdjustedPosition;
            for(u32 i : it){
                if(it.entity(i).has<SelectedForEditing>()){
                    continue;
                }
                for(int j = 0; j < pvcs[i].vals.size(); j++){
                    v2d nodePosition = pvcs[i].vals[j];
                    v2d v = nodePosition - mousePosition;
                    float distance = v2d_magnitude(v);
                    if(distance <= distanceForSelectionTolerance){
                        flecs::world world = it.world();
                        deselectAll(world);
                        it.world().defer_begin();
                        it.entity(i).add<SelectedForEditing>();
                        it.world().defer_end();
                        break;
                    }
                }

            }
        }
    });

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
            flecs::world world = it.world();
            deselectAll(world);
        }
        
    }
}

