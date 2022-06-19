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
#include "editingFunctionality.h"
#include "intersections.h"


static void deselectAll(flecs::world &ecs){
    ecs.defer_begin();

    auto f = ecs.filter<SelectedForEditing>();
    f.each([&](flecs::entity e, SelectedForEditing s){
        e.remove<SelectedForEditing>();
    });


    ecs.defer_end();
}


static void EndEditingSelectedPlatformNode(flecs::world &ecs){
    auto f = ecs.filter<PlatformNodeCollection, SelectedForEditing>();
    ecs.defer_begin();

    f.each([&](flecs::entity e, PlatformNodeCollection pnc, SelectedForEditing selected){
        e.remove<SelectedForEditing>();
    });

    ecs.defer_begin();
}


static void createAndSelectPlatformNodeEntity(flecs::world &ecs, PlatformNodeCollection pnc){

   // ecs.defer_begin();

    flecs::entity pncEntity = ecs.entity();

    pncEntity.set<Position>((Position){0,0});
    pncEntity.set<PlatformNodeCollection>(pnc);
    pncEntity.add<SelectedForEditing>();

   // ecs.defer_end();
}


//
//
// SYSYEMS
//
//

// editor should have select state
void selectPncSystem(flecs::iter &it, MouseState *mouseStates){
    flecs::world ecs = it.world();
    auto f = ecs.filter<Position, PlatformNodeCollection>();

    for(int i : it){
        if(mouseStates[i].lmbCurrentState == INPUT_IS_JUST_RELEASED){
            Position mousePosition = {mouseStates[i].worldPosition.x, mouseStates[i].worldPosition.y};

            f.iter([&](flecs::iter &it, Position *ps, PlatformNodeCollection *pncs){
                float distanceForSelectionTolerance = 8.0f; // how far away can user click

                for(u32 j : it){
                    if(it.entity(j).has<SelectedForEditing>()){
                        continue; //ignore
                    }
                    for(int k = 0; k < pncs[j].vals.size()- 1; k++){
                        v2d nodePosition = v2d_add(ps[j], pncs[j].vals[k]);
                        v2d nodePosition2 = v2d_add(ps[j], pncs[j].vals[k+1]);

                        if(PointIntersectLineWithTolerance(nodePosition, nodePosition2, mousePosition, distanceForSelectionTolerance)){
                            flecs::world world = it.world();
                            deselectAll(world);
                            it.world().defer_begin();
                            it.entity(j).add<SelectedForEditing>();
                            it.entity(i).add<EditMode::AppendNodeMode>();
                            it.entity(i).remove<EditMode::SelectPncMode>();
                            it.world().defer_end();
                            break;
                        }
                    }

                }
            });
        }
    }
}

/** adding new pnc
 * @brief  if(NoneSelected){
                PlatformNodeCollection pnc;
                pnc.vals.push_back((mousePosition));
                createAndSelectPlatformNodeEntity(ecs, pnc);
            }
 * 
 * @param it 
 * @param inputs 
 * @param mouseStates 
 */


// CreatePncMode
void createPncSystem(flecs::iter &it, MouseState *mouseStates){
    auto world = it.world();
    for(int i : it){
        if(mouseStates[i].lmbCurrentState != INPUT_IS_JUST_RELEASED){
            continue;
        }
        Position mousePosition = mouseStates[i].worldPosition;

        PlatformNodeCollection pnc;
        pnc.vals.push_back((mousePosition));
        createAndSelectPlatformNodeEntity(world, pnc);
        flecs::entity e = it.entity(i);
        // left off here !
        world.defer_begin();

        e.remove<EditMode::CreatePncMode>();
        e.add<EditMode::AppendNodeMode>();

        world.defer_end();
    }
}

// AddVertexMode
void AppendNodeToSelectedPncSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates){
    for(int i : it){
        auto ecs = it.world();

        Position mousePosition = mouseStates[i].worldPosition;

        auto f = ecs.filter<Position, PlatformNodeCollection, SelectedForEditing>();
        f.iter([&](flecs::iter &it, Position *ps,PlatformNodeCollection *pnc, SelectedForEditing *selected){
            for(u32 j : it){
                Position tailVertex = pnc[j].vals.at(pnc[j].vals.size()-1);
                Position localPosition = v2d_sub(mousePosition, ps[j]);
                Position tailToMousePosition = v2d_sub(localPosition, tailVertex);

                if(inputIsPressed(inputs[i], "edit-angle-snap")){
                    if(abs(tailToMousePosition.x) < abs(tailToMousePosition.y)){
                        localPosition.x = pnc[j].vals.at(pnc[j].vals.size()-1).x;
                    }
                    else {
                        localPosition.y = pnc[j].vals.at(pnc[j].vals.size()-1).y;
                    }
                }
                pnc[j].vals.push_back(localPosition); 
            }
        });
    }
}

void DeleteSelectedPncSystem(flecs::iter &it, Position *positions, PlatformNodeCollection *pncs, SelectedForEditing *s){
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

void SelectNodeSystem(flecs::iter &it, SelectedForEditing *s, Position *positions, PlatformNodeCollection *pncs){
    float distanceForSelectionTolerance = 5.0f;
    auto f = it.world().filter<MouseState>();

    
    f.each([&](flecs::entity e, MouseState ms){
        if(ms.lmbCurrentState == INPUT_IS_JUST_RELEASED){
            v2d mousePosition = ms.worldPosition;
            for(u32 i : it){
                for(int j = 0; j < pncs[i].vals.size(); j++){
                    if(PointIntersectPointWithTolerance(mousePosition, pncs[i].vals[j], distanceForSelectionTolerance)){
                        SelectedForEditingNode sn;
                        sn.idx = j;
                        it.world().defer_begin();

                        it.entity(i).set<SelectedForEditingNode>(sn);
                        e.add<EditMode::MoveNodeMode>();
                        e.remove<EditMode::SelectNodeMode>();

                        it.world().defer_end();
                    }
                }
            }
        }
    });

    
}


void moveNodeSystem(flecs::iter &it, MouseState *mouseStates){
    auto f = it.world().filter<Position, PlatformNodeCollection, SelectedForEditingNode>();

    for(auto i : it){
        Position mousePosition = mouseStates[i].worldPosition;

        f.each([&](flecs::entity e, Position p, PlatformNodeCollection pnc, SelectedForEditingNode node){
            Position mousePositionTransformed = v2d_add(mousePosition, p);
            pnc.vals[node.idx] = mousePositionTransformed = mousePositionTransformed;
        });
    }   
    
}


void DeselectInputSystem(flecs::iter &it, Input *inputs){
    
    for(int i : it){
        
        if(inputIsJustReleased(inputs[i], "deselect")){
            flecs::world world = it.world();
            deselectAll(world);
            
            world.defer_begin();

            flecs::entity e = it.entity(i);

            e.remove<EditMode::AppendNodeMode>();
            e.remove<EditMode::CreatePncMode>();
            e.remove<EditMode::MoveNodeMode>();
            e.remove<EditMode::SelectNodeMode>();
            e.add<EditMode::SelectPncMode>();


            world.defer_end();
        }
        
    }
}

