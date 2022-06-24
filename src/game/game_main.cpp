#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include "animation.h"
#include "animation_util.h"
#include "ints.h"
#include "spriteSheets.h"
#include "spriteSheet_util.h"
#include "velocity.h"
#include "movement.h"
#include "input.h"
#include "flecs.h"
#include "ray2d.h"
#include "states.h"
#include "solid_rect.h"
#include "collisions.h"
#include "debug_display.h"
#include "camera.h"
#include "timestep.h"
#include "render.h"
#include "ParallaxSprite.h"
#include "parallaxSpriteProcessing.h"
#include "mouseState.h"
#include "mouseStateProcessing.h"
#include "loadSave.h"
#include "editingFunctionality.h"
#define V2D_IMPLEMENTATION
#include "v2d.h"


SDL_Renderer *gRenderer;
SDL_Window *gWindow;

int gScreenWidth = 1920;
int gScreenHeight = 1080;

SpriteSheet gSpriteSheets[MAX_SPRITE_SHEETS]; 
size_t gNumSpriteSheets = 0;

flecs::entity gEditorEntity;



void registerSystems(flecs::world &ecs){

    // INPUT GATHER
    ecs.system<Input>()
        .kind(flecs::PreUpdate)
        .iter(Input_update_input_button_states_System);
    
    ecs.system<MouseState>()
        .kind(flecs::PreUpdate)
        .iter(mouseStateSetterSystem);
    
    // GAMEPLAY
    ecs.system<Velocity, GroundSpeed, Input, StateCurrPrev, Angle>("keyStateVelocitySetter")
        .kind(flecs::PreUpdate)
        .iter(movement_GrounSpeed_Velocity_update_from_Input_and_Phyics_System);

    ecs.system<AnimatedSprite>("AnimatedSpritePlay")
        .kind(flecs::OnUpdate)
        .iter(anim_update_AnimatedSprite_accumulate_time_to_increment_frame_System);

    ecs.system<AnimatedSprite, Position, Angle>("renderingAnimatedSprites")
        .kind(flecs::OnStore)
        .iter(anim_render_AnimatedSprites_System);

    ecs.system<AnimatedSprite, KeyboardState>("keyStateAnimationSpriteState")
        .kind(flecs::OnUpdate)
        .iter(anim_update_KeyboardState_AnimatedSprite_set_animation_System);

    ecs.system<Velocity, Position>("move")
        .kind(flecs::OnUpdate)
        .iter(movement_apply_velocity_to_position_System);

    ecs.system<AnimatedSprite, Input>()
        .kind(flecs::OnUpdate)
        .iter(anim_update_AnimatedSprite_flip_based_on_Input_System);

    ecs.system<AnimatedSprite, Velocity, StateCurrPrev>()
        .kind(flecs::OnUpdate)
        .iter(anim_update_AnimatedSprite_set_animation_based_on_speed_and_state__System);

    ecs.system<Velocity, StateCurrPrev>("gravity system")
        .kind(flecs::OnUpdate)
        .iter(movement_velocity_apply_gravity_System);


    // COLLISONS
    ecs.system<Position, Sensors, Velocity, GroundSpeed, GroundMode, StateCurrPrev, Angle>("collision")
        .kind(flecs::PostUpdate)
        .iter(collisions_Sensors_PlatformPaths_update_Position_System);

    // RENDERING

    ecs.system<>()
        .kind(flecs::PreFrame)
        .iter(renderFrameStartSystem);

    ecs.system<Position, Sensors, GroundMode>()
        .kind(flecs::OnStore)
        .iter(renderSensorsSystem);

    ecs.system<SelectedForEditing, Position, PlatformPath>()
        .kind(flecs::OnStore)
        .iter(renderSelectedPlatformNodeSystem);
    
    ecs.system<Position, PlatformPath>()
        .term<SelectedForEditing>().oper(flecs::Not)
        .kind(flecs::OnStore)
        .iter(renderUnselectedPlatformNodeSystem);

    ecs.system<SelectedForEditingNode, Position, PlatformPath>()
        .kind(flecs::OnStore)
        .iter(renderSelectedPlatformNodeNodesSystem);

    ecs.system<Position, PlatformPath>()
        .term<SelectedForEditingNode>().oper(flecs::Not)
        .kind(flecs::OnStore)
        .iter(renderUnselectedPlatformNodeNodesSystem);

    ecs.system<>()
        .kind(flecs::OnStore)
        .iter(cam_zoom_render_frame_start_System);

    ecs.system<Input, MouseState>()
        .kind(flecs::OnStore)
        .term<EditMode::PlatformPathNodeAppendMode>()
        .iter(renderUncommitedLinesToPlaceSystem);

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(cam_input_zoom_System);

    ecs.system<Position, ParallaxSprite>()
        .kind(flecs::OnUpdate)
        .iter(render_ParallaxSprite_System);

    ecs.system<>()
        .kind(flecs::PostFrame)
        .iter(renderEndFrameSystem);


    // EDITING
    ecs.system<Input>()
        .kind(flecs::OnStore)
        .iter(load_PlatformPaths_on_load_button_release_System);

    ecs.system<Input>()
        .kind(flecs::OnStore)
        .iter(save_PlatformPaths_on_save_button_release_System);

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(
            ed_SelectedForEditing_tag_remove_all_and_set_default_EditMode_on_deselect_button_release_System
        );

    ecs.system<MouseState>()
        .kind(flecs::OnUpdate)
        .term<EditMode::PlatformPathCreateMode>()
        .iter(
            ed_PlatformPath_create_entity_on_click_System
        );

    ecs.system<MouseState>()
        .kind(flecs::OnUpdate)
        .term<EditMode::PlatformPathSelectMode>() //should be AND oper by default?
        .iter(
            ed_PlatformPath_select_on_click_System
        );
    
    ecs.system<Input, MouseState>()
        .kind(flecs::OnUpdate)
        .term<EditMode::PlatformPathNodeAppendMode>()
        .iter(
            ed_PlatformPath_node_append_to_selected_on_click_System
        );
        
    ecs.system<Position, PlatformPath, SelectedForEditing>()
        .kind(flecs::OnUpdate)
        .iter(
            ed_PlatformPath_destruct_selected_on_delete_button_release_System
        );
    
    ecs.system<MouseState>()
        .kind(flecs::OnUpdate)
        .term<EditMode::PlatformPathNodeSelectMode>()
        .iter(
            ed_PlatformPath_node_select_on_click_System
        );
    
    ecs.system<MouseState>()
        .kind(flecs::OnUpdate)
        .term<EditMode::PlatformPathNodeMoveMode>()
        .iter(
            ed_PlatformPath_node_move_on_drag_System
        );

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(
            ed_EditMode_change_depending_on_Input_release
        );
    
}





int main(){
    MouseState mouseState;
    mouseState.lmbCurrentState = INPUT_IS_NOT_PRESSED;
    mouseState.rmbCurrentState = INPUT_IS_NOT_PRESSED;

    bool quit = false;
    /**
     * FLECS SETUP
     * 
     */

    flecs::world world;
    world.set<flecs::Rest>({});
    flecs::entity pinkGuyEntity = world.entity("PinkGuy");


    /**
     * SDL SETUP
     * 
     */
    SDL_Init(SDL_INIT_VIDEO);
    gWindow = SDL_CreateWindow("title",0,0, gScreenWidth,gScreenHeight, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    SDL_Event event;

    /**
     * SETUP BACKGROUND
     * 
     */
    u32 bgSpriteId = SpriteSheet_util_create(
        "res/checkerboard-bg.png", 
        1, 
        1, 
        "checkerboard-background"
    );
    ParallaxSprite parallaxSprite;
    parallaxSprite.name = "checkerboard-background-px";
    parallaxSprite.scale = 0.1f;
    parallaxSprite.spriteSheetId = bgSpriteId;

    Position parallaxSpritePosition = {gScreenWidth/2.0f, gScreenHeight/2.0f};

    flecs::entity pxBgEntity = world.entity("Parallax Background Entity");
    pxBgEntity.set<Position>(parallaxSpritePosition);
    pxBgEntity.set<ParallaxSprite>(parallaxSprite);




    /**
     * ANIMATED SPRITE SETUP
     * 
     */
     pinkGuyEntity.add<AnimatedSprite>();

    std::string filename = "res/pink-monster-animation-transparent.png";
    std::string animatedSpriteName =  "pink-monster-animation";
    u32 spriteSheetId = SpriteSheet_util_create(filename, 9, 15, animatedSpriteName);
    AnimatedSprite animatedSprite = anim_util_AnimatedSprite_create(spriteSheetId);

    Animation walkAnimation = anim_util_Animation_init({15,16,17,18,19,20}, 12.0f, true, "walk");
    Animation runAnimation = anim_util_Animation_init({30,31,32,33,34,35}, 12.0f, true, "run");
    Animation standingAttackAnimation = anim_util_Animation_init({45,46,47,48}, 12.0f, false, "stand-attack");
    Animation idleAnimation = anim_util_Animation_init({75,76,77,78,77,76}, 12.0f, true, "idle");
    Animation jumpAnimation = anim_util_Animation_init({120,121,122,123,124,125}, 12.0f, true, "jump");
    

    anim_util_AnimatedSprite_add_Animation(animatedSprite, walkAnimation);
    anim_util_AnimatedSprite_add_Animation(animatedSprite, runAnimation);
    anim_util_AnimatedSprite_add_Animation(animatedSprite, idleAnimation);
    anim_util_AnimatedSprite_add_Animation(animatedSprite, standingAttackAnimation);
    anim_util_AnimatedSprite_add_Animation(animatedSprite, jumpAnimation);


    
    Input pinkGuyInput;
    Input_append_new_input_button_state_mapped_to_sdlScancode(pinkGuyInput, "left", SDL_SCANCODE_A);
    Input_append_new_input_button_state_mapped_to_sdlScancode(pinkGuyInput, "right", SDL_SCANCODE_D);
    Input_append_new_input_button_state_mapped_to_sdlScancode(pinkGuyInput, "jump", SDL_SCANCODE_SPACE);

    flecs::entity editorEntity = world.entity();
    Input editorInput;
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "zoom-in", SDL_SCANCODE_UP);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "zoom-out", SDL_SCANCODE_DOWN);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "zoom-reset", SDL_SCANCODE_R);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "save", SDL_SCANCODE_1);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "load", SDL_SCANCODE_2);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "deselect", SDL_SCANCODE_3);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "platform-path-create-mode-enter", SDL_SCANCODE_4);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "platform-path-select-mode-enter", SDL_SCANCODE_5);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "platform-path-node-append-mode-enter", SDL_SCANCODE_6);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "platform-path-node-select-mode-enter", SDL_SCANCODE_7);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "platform-path-node-move-mode-enter", SDL_SCANCODE_8);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "edit-angle-snap", SDL_SCANCODE_LSHIFT);
    Input_append_new_input_button_state_mapped_to_sdlScancode(editorInput, "delete", SDL_SCANCODE_DELETE);

    editorEntity.set<Input>(editorInput);
    editorEntity.set<MouseState>(mouseState);
    editorEntity.add<EditMode::PlatformPathSelectMode>();

    pinkGuyEntity.set<Input>(pinkGuyInput);

    Sensors pinkGuySensors;

    pinkGuySensors.rays[LF_SENSOR].startingPosition = (Position){-8.0f, 0.0f};
    pinkGuySensors.rays[LF_SENSOR].distance = 16.0f;
    pinkGuySensors.rays[RF_SENSOR].startingPosition = (Position){8.0f, 0.0f};
    pinkGuySensors.rays[RF_SENSOR].distance = 16.0f;

    pinkGuySensors.rays[LW_SENSOR].startingPosition = (Position){0.0f, 8.0f};
    pinkGuySensors.rays[LW_SENSOR].distance = 8.0f;
    pinkGuySensors.rays[RW_SENSOR].startingPosition = (Position){0.0f, 8.0f};
    pinkGuySensors.rays[RW_SENSOR].distance = 8.0f;




    StateCurrPrev state;
    state.currentState = STATE_ON_GROUND;
    state.prevState = STATE_ON_GROUND;

    pinkGuyEntity.set<AnimatedSprite>(animatedSprite);
    pinkGuyEntity.set<Position>((Position){640.0f/2.0f,480.0f/2.0f - 400});
    pinkGuyEntity.set<Velocity>((Velocity){0,0});
    pinkGuyEntity.set<Angle>((Angle){0.0f});
    pinkGuyEntity.set<StateCurrPrev>(state);
    pinkGuyEntity.set<Sensors>(pinkGuySensors);
    pinkGuyEntity.set<GroundSpeed>((GroundSpeed){0.0f});
    pinkGuyEntity.set<GroundMode>(FLOOR_GM);
    
    registerSystems(world);

    
    load_PlatformPaths(world);

    gTimeStep = ts_TimeStep_init(60.0f);

    // main loop
    while(!quit){

        ts_TimeStep_start_ticks_set_to_current_ticks(gTimeStep);

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
                break;
            }
        }
        gKeyStates = (u8 *)SDL_GetKeyboardState(nullptr);
   

        gCameraPosition.x = pinkGuyEntity.get<Position>()->x;
        gCameraPosition.y = pinkGuyEntity.get<Position>()->y;


        world.progress();
    }
}