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


static void 
internal_SelectedForEditing_tag_remove_on_all_entities_deffered(flecs::world &ecs){
    ecs.defer_begin();

    auto f = ecs.filter<SelectedForEditing>();
    f.each([&](flecs::entity e, SelectedForEditing s){
        e.remove<SelectedForEditing>();
    });


    ecs.defer_end();
}



static void 
internal_PlatformPath_entity_create_and_init(flecs::world &ecs, PlatformPath platformPath){

    flecs::entity platformPathEntity = ecs.entity();

    platformPathEntity.set<Position>((Position){0,0});
    platformPathEntity.set<PlatformPath>(platformPath);
    platformPathEntity.add<SelectedForEditing>();
}


//
//
// SYSYEMS
//
//

void 
ed_PlatformPath_create_entity_on_click_System(flecs::iter &it, MouseState *mouseStates){
    auto world = it.world();
    for(int i : it){
        if(mouseStates[i].lmbCurrentState != INPUT_IS_JUST_RELEASED){
            continue;
        }
        Position mousePosition = mouseStates[i].worldPosition;

        PlatformPath platformPath;
        platformPath.nodes.push_back((mousePosition));
        platformPath.isCircular = false;
        internal_PlatformPath_entity_create_and_init(world, platformPath);
        
        flecs::entity e = it.entity(i);

        world.defer_begin();

        e.remove<EditMode::PlatformPathCreateMode>();
        e.add<EditMode::PlatformPathNodeAppendMode>();

        world.defer_end();
    }
}
// editor should have select state
void 
ed_PlatformPath_select_on_click_System(flecs::iter &it, MouseState *mouseStates){
    flecs::world ecs = it.world();
    auto f = ecs.filter<Position, PlatformPath>();

    for(int i : it){
        if(mouseStates[i].lmbCurrentState == INPUT_IS_JUST_RELEASED){
            Position mousePosition = mouseStates[i].worldPosition;

            bool didSelect = false; // can't remove and add tags to editor entity inside below inline function

            f.iter([&](flecs::iter &it, Position *ps, PlatformPath *platformPaths){
                float distanceForSelectionTolerance = 8.0f; // how far away can user click

                for(u32 j : it){
                    if(it.entity(j).has<SelectedForEditing>()){
                        continue; //ignore
                    }
                    for(int k = 0; k < platformPaths[j].nodes.size()- 1; k++){
                        v2d nodePosition = v2d_add(ps[j], platformPaths[j].nodes[k]);
                        v2d nodePosition2 = v2d_add(ps[j], platformPaths[j].nodes[k+1]);

                        if(PointIntersectLineWithTolerance(nodePosition, nodePosition2, mousePosition, distanceForSelectionTolerance)){
                            flecs::world world = it.world();

                            it.world().defer_begin();

                            auto f = ecs.filter<SelectedForEditing>();
                            f.each([&](flecs::entity e, SelectedForEditing s){
                                e.remove<SelectedForEditing>();
                            });

                            it.entity(j).add<SelectedForEditing>();

                            it.world().defer_end();

                            didSelect = true;
                        }
                    }

                }
            });

            if(didSelect){
                it.world().defer_begin();

                flecs::entity currentEditorEntity = it.entity(i);
                currentEditorEntity.remove<EditMode::PlatformPathSelectMode>();
                currentEditorEntity.add<EditMode::PlatformPathNodeAppendMode>();

                it.world().defer_end();
            }
        }
    }
}


void 
ed_PlatformPath_node_append_to_selected_on_click_System(flecs::iter &it, Input *inputs, MouseState *mouseStates){
    for(int i : it){
        if(mouseStates[i].lmbCurrentState != INPUT_IS_JUST_RELEASED){
            continue;
        }
        auto ecs = it.world();

        Position mousePosition = mouseStates[i].worldPosition;

        auto f = ecs.filter<Position, PlatformPath, SelectedForEditing>();
        f.iter([&](flecs::iter &it, Position *ps,PlatformPath *platformPath, SelectedForEditing *selected){
            for(u32 j : it){
                Position tailVertex = platformPath[j].nodes.at(platformPath[j].nodes.size()-1);
                Position localPosition = v2d_sub(mousePosition, ps[j]);
                Position tailToMousePosition = v2d_sub(localPosition, tailVertex);

                if(Input_is_pressed(inputs[i], "edit-angle-snap")){
                    if(abs(tailToMousePosition.x) < abs(tailToMousePosition.y)){
                        localPosition.x = platformPath[j].nodes.at(platformPath[j].nodes.size()-1).x;
                    }
                    else {
                        localPosition.y = platformPath[j].nodes.at(platformPath[j].nodes.size()-1).y;
                    }
                }
                platformPath[j].nodes.push_back(localPosition); 
            }
        });
    }
}

void 
ed_PlatformPath_destruct_selected_on_delete_button_release_System(flecs::iter &it, Position *positions, PlatformPath *platformPaths, SelectedForEditing *s){
    flecs::world world = it.world();
    auto f = world.filter<Input>();
    bool shouldDelete = false;
    f.each([&](flecs::entity e, Input input){
        if(Input_is_just_released(input, "delete")){
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

void 
ed_PlatformPath_node_select_on_click_System(flecs::iter &it, MouseState *mouseStates){
    float distanceForSelectionTolerance = 5.0f;
    auto f = it.world().filter<Position, PlatformPath>();

    bool didSelect = false;

    for(u32 i : it){
        f.each([&](flecs::entity e, Position position, PlatformPath platformPath){
            if(mouseStates[i].lmbCurrentState == INPUT_IS_JUST_PRESSED){
                v2d mousePosition = mouseStates[i].worldPosition;
                for(int j = 0; j < platformPath.nodes.size(); j++){
                    Position worldPlatformPathNodePosition = v2d_add(position, platformPath.nodes[j]);
                    
                    if(PointIntersectPointWithTolerance(mousePosition, worldPlatformPathNodePosition, distanceForSelectionTolerance)){
                        SelectedForEditingNode sn;
                        sn.idx = j;
                        it.world().defer_begin();

                        e.set<SelectedForEditingNode>(sn);
                        

                        it.world().defer_end();

                        didSelect = true;
                    }
                }
                
            }
        });

        if(didSelect){
            it.world().defer_begin();

            it.entity(i).add<EditMode::PlatformPathNodeMoveMode>();
            it.entity(i).remove<EditMode::PlatformPathNodeSelectMode>();

            it.world().defer_end();
        }
    }   
    
}


void 
ed_PlatformPath_node_move_on_drag_System(flecs::iter &it, MouseState *mouseStates){
    auto filter_platformPathsWithSelectedNode = it.world().filter<Position, PlatformPath, SelectedForEditingNode>();
    auto filter_platformPaths = it.world().filter<Position, PlatformPath>();

    for(auto i : it){
        if(mouseStates[i].lmbCurrentState == INPUT_IS_JUST_RELEASED){
            it.world().defer_begin();


            filter_platformPathsWithSelectedNode
                .iter([&](flecs::iter &it, Position *positions, PlatformPath *platformPaths, SelectedForEditingNode *selectedNodes){
                    for(u32 j : it){
                        it.entity(j).remove<SelectedForEditingNode>();
                    }
            });

            it.world().defer_end();
        }
        else if(mouseStates[i].lmbCurrentState == INPUT_IS_JUST_PRESSED){
            float distanceForSelectionTolerance = 5.0f;

            v2d mousePosition = mouseStates[i].worldPosition;

            filter_platformPaths
                .each([&](flecs::entity e, Position position, PlatformPath platformPath){
                    for(int j = 0; j < platformPath.nodes.size(); j++){
                        Position worldPlatformPathNodePosition = v2d_add(position, platformPath.nodes[j]);
                        
                        if(PointIntersectPointWithTolerance(mousePosition, worldPlatformPathNodePosition, distanceForSelectionTolerance)){
                            SelectedForEditingNode sn;
                            sn.idx = j;
                            it.world().defer_begin();
                
                            e.set<SelectedForEditingNode>(sn);

                            it.world().defer_end();
                            
                        }
                    }
                });       
        }
        else if(mouseStates[i].lmbCurrentState == INPUT_IS_PRESSED){
            Position mousePosition = mouseStates[i].worldPosition;

            filter_platformPathsWithSelectedNode.iter([&](flecs::iter &it, Position *positions, PlatformPath *platformPaths, SelectedForEditingNode *selectedNodes){
                for(u32 j : it){
                    Position mousePositionTransformed = v2d_add(mousePosition, positions[j]);
                    platformPaths[j].nodes[selectedNodes[j].idx] = mousePositionTransformed;
                }
            });
        }
    }   
    
}


void 
ed_SelectedForEditing_tag_remove_all_and_set_default_EditMode_on_deselect_button_release_System(flecs::iter &it, Input *inputs){
    
    for(int i : it){
        
        if(Input_is_just_released(inputs[i], "deselect")){
            flecs::world world = it.world();

            internal_SelectedForEditing_tag_remove_on_all_entities_deffered(world);
            
            world.defer_begin();

            flecs::entity e = it.entity(i);

            e.remove<EditMode::PlatformPathNodeAppendMode>();
            e.remove<EditMode::PlatformPathCreateMode>();
            e.remove<EditMode::PlatformPathNodeMoveMode>();
            e.remove<EditMode::PlatformPathNodeSelectMode>();
            e.add<EditMode::PlatformPathSelectMode>();


            world.defer_end();
        }
        
    }
}



template<typename T> void 
internal_EditMode_tags_remove_all_from_entity_except_T_deferred(flecs::entity entity){
    entity.world().defer_begin();

    entity.remove<EditMode::PlatformPathCreateMode>();
    entity.remove<EditMode::PlatformPathSelectMode>();
    entity.remove<EditMode::PlatformPathNodeAppendMode>();
    entity.remove<EditMode::PlatformPathNodeSelectMode>();
    entity.remove<EditMode::PlatformPathNodeMoveMode>();

    entity.add<T>();


    entity.world().defer_end();
}


void 
ed_EditMode_change_depending_on_Input_release(flecs::iter &it, Input *inputs){
    flecs::world world = it.world();

    for(u32 i : it){

        if(Input_is_just_released(inputs[i], "platform-path-create-mode-enter")){
            internal_EditMode_tags_remove_all_from_entity_except_T_deferred<EditMode::PlatformPathCreateMode>(it.entity(i));
            internal_SelectedForEditing_tag_remove_on_all_entities_deffered(world);
        }

        else if(Input_is_just_released(inputs[i], "platform-path-select-mode-enter")){
            internal_EditMode_tags_remove_all_from_entity_except_T_deferred<EditMode::PlatformPathSelectMode>(it.entity(i));
            internal_SelectedForEditing_tag_remove_on_all_entities_deffered(world);
        }

        else if(Input_is_just_released(inputs[i], "platform-path-node-append-mode-enter")){
            internal_EditMode_tags_remove_all_from_entity_except_T_deferred<EditMode::PlatformPathNodeAppendMode>(it.entity(i));
        }

        else if(Input_is_just_released(inputs[i], "platform-path-node-select-mode-enter")){
            internal_EditMode_tags_remove_all_from_entity_except_T_deferred<EditMode::PlatformPathNodeSelectMode>(it.entity(i));
            internal_SelectedForEditing_tag_remove_on_all_entities_deffered(world);
        }

        else if(Input_is_just_released(inputs[i], "platform-path-node-move-mode-enter")){
            internal_EditMode_tags_remove_all_from_entity_except_T_deferred<EditMode::PlatformPathNodeMoveMode>(it.entity(i));
            internal_SelectedForEditing_tag_remove_on_all_entities_deffered(world);
        }
    }
}