#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include "animation.h"
#include "animation_util.h"
#include "animation_render.h"
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
#include "debug_render.h"
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

    auto custom_phase_input_gather = ecs.entity()
        .add(flecs::Phase);

    auto custom_phase_speeds_set = ecs.entity()
        .add(flecs::Phase)
        .depends_on(custom_phase_input_gather);

    auto custom_phase_pre_movement = ecs.entity()
        .add(flecs::Phase)
        .depends_on(custom_phase_speeds_set);

    auto custom_phase_collision_wall = ecs.entity()
        .add(flecs::Phase)
        .depends_on(custom_phase_speeds_set);

    auto custom_phase_movement = ecs.entity()
        .add(flecs::Phase)
        .depends_on(custom_phase_collision_wall);

    auto custom_phase_pre_collision_floor = ecs.entity()
        .add(flecs::Phase)
        .depends_on(custom_phase_movement);

    auto custom_phase_collision_floor = ecs.entity()
        .add(flecs::Phase)
        .depends_on(custom_phase_pre_collision_floor);

    auto custom_phase_control_lock_activate_slip = ecs.entity()
        .add(flecs::Phase)
        .depends_on(custom_phase_collision_floor);

    auto custom_phase_animation = ecs.entity()
        .add(flecs::Phase)
        .depends_on(custom_phase_collision_floor);

    auto custom_phase_render = ecs.entity()
        .add(flecs::Phase)
        .depends_on(custom_phase_animation);


    ecs.system<Input>()
        .kind(custom_phase_input_gather)
        .iter(
            Input_update_input_button_states_System
        );
    
    ecs.system<MouseState>()
        .kind(custom_phase_input_gather)
        .iter(
            MouseState_update_System
        );
    
    ecs.system<Velocity, GroundSpeed, Input, StateCurrPrev, Angle>("keyStateVelocitySetter")
        .kind(custom_phase_speeds_set)
        .iter(
            movement_GrounSpeed_Velocity_update_from_Input_and_Phyics_System
        );
    
    ecs.system<Velocity, StateCurrPrev>("gravity system")
        .kind(custom_phase_speeds_set)
        .iter(
            movement_velocity_apply_gravity_System
        );

    ecs.system<SensorCollection, Velocity, GroundMode>("wall sensor distance set based on velocity")
        .kind(custom_phase_pre_movement)
        .iter(
            collisions_Sensors_wall_sensors_set_distance_from_velocity_System
        );
    
    ecs.system<SensorCollection, Angle>("wall sensor height set based on angle")
        .kind(custom_phase_pre_movement)
        .iter(
            collisions_Sensors_wall_set_height_from_Angle_System
        );


    ecs.system<ControlLockTimer, StateCurrPrev>()
        .kind(custom_phase_pre_movement)
        .iter(
            movement_ControlLockTimer_update_when_on_ground
        );

    ecs.system<Velocity, Position>("move")
        .kind(custom_phase_movement)
        .iter(
            movement_apply_velocity_to_position_System
        );


    ecs.system<AnimatedSprite, Input>()
        .kind(custom_phase_animation)
        .iter(
            anim_update_AnimatedSprite_flip_based_on_Input_System
        );
    
    ecs.system<AnimatedSprite, Input>()
        .kind(custom_phase_animation)
        .iter(
            anim_update_set_jump_animation_on_jump_input_System
        );

    ecs.system<AnimatedSprite, GroundSpeed, StateCurrPrev>()
        .kind(custom_phase_animation)
        .iter(
            anim_update_AnimatedSprite_set_animation_based_on_ground_speed_System
        );

    


   
    

    ecs.system<Position, SensorCollection, Velocity, GroundSpeed, GroundMode, StateCurrPrev, Angle>("collision wall update position")
        .kind(custom_phase_collision_wall)
        .iter(collisions_Sensors_wall_update_Position_System);
    

    ecs.system<Position, Velocity, SensorCollection, GroundMode, StateCurrPrev, Angle, CollisionResultPlatformPathFloorSensor>("collision get results")
        .kind(custom_phase_pre_collision_floor)
        .iter(collisions_floor_sensors_intersect_platform_paths_set_result_System);
        
    ecs.system<
        Position, 
        Angle, 
        SensorCollection, 
        GroundMode, 
        StateCurrPrev, 
        CollisionResultPlatformPathFloorSensor
    >("collision")
        .kind(custom_phase_collision_floor)
        .iter(collisions_position_rotation_align_based_on_collision_result_System);

    // ecs.system<GroundSpeed, Angle, StateCurrPrev>("slip and add controllocktimer when too steep and too slow")
    //     .kind(custom_phase_control_lock_activate_slip)
    //     .term<ControlLockTimer>().oper(flecs::Not)
    //     .iter(
    //         movement_GroundSpeed_zero_ControlLockTimer_add_based_on_too_steep_angle_System
    //     );


    ecs.system<Position, Velocity, CollisionResultPlatformPathFloorSensor>()
        .kind(custom_phase_render)
        .iter(
            cam_set_position_based_on_velocity_System        
        );

    

    

    ecs.system<AnimatedSprite>("AnimatedSpritePlay")
        .kind(custom_phase_animation)
        .iter(
            anim_update_AnimatedSprite_accumulate_time_to_increment_frame_System
        );

    ecs.system<AnimatedSprite, KeyboardState>("keyStateAnimationSpriteState")
        .kind(custom_phase_animation)
        .iter(
            anim_update_KeyboardState_AnimatedSprite_set_animation_System
        );

    ecs.system<>()
        .kind(flecs::PreFrame)
        .iter(render_frame_start_System
    );
    
    ecs.system<AnimatedSprite, Position, Angle>("renderingAnimatedSprites")
        .kind(custom_phase_render)
        .iter(
            anim_render_AnimatedSprites_System
    );

    ecs.system<Position, SensorCollection, Angle>()
        .kind(custom_phase_render)
        .iter(
            renderSensorsSystem
        );

    ecs.system<SelectedForEditing, Position, PlatformPath>()
        .kind(custom_phase_render)
        .iter(
            renderSelectedPlatformNodeSystem
        );
    
    ecs.system<Position, PlatformPath>()
        .term<SelectedForEditing>().oper(flecs::Not)
        .kind(custom_phase_render)
        .iter(
            renderUnselectedPlatformNodeSystem
        );

    ecs.system<SelectedForEditingNode, Position, PlatformPath>()
        .kind(custom_phase_render)
        .iter(
            renderSelectedPlatformNodeNodesSystem
        );

    ecs.system<Position, PlatformPath>()
        .term<SelectedForEditingNode>().oper(flecs::Not)
        .kind(custom_phase_render)
        .iter(
            renderUnselectedPlatformNodeNodesSystem
        );

    ecs.system<>()
        .kind(custom_phase_render)
        .iter(
            cam_zoom_render_frame_start_System
        );

    ecs.system<Input, MouseState>()
        .kind(custom_phase_render)
        .term<EditMode::PlatformPathNodeAppendMode>()
        .iter(
            renderUncommitedLinesToPlaceSystem
        );

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(
            cam_input_zoom_System
        );

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(
            ts_TimeStep_adjust_on_input_System
        );

    ecs.system<Position, ParallaxSprite>()
        .kind(flecs::OnUpdate)
        .iter(
            render_ParallaxSprite_System
        );

    ecs.system<>()
        .kind(flecs::PostFrame)
        .iter(
            render_end_frame_System
        );


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

    std::string filename = "res/pink-monster-animation-transparent-2.png";
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

    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "zoom-in", SDL_SCANCODE_UP
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "zoom-out", SDL_SCANCODE_DOWN
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "zoom-reset", SDL_SCANCODE_R
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "frame-rate-increase", SDL_SCANCODE_RIGHT
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "frame-rate-decrease", SDL_SCANCODE_LEFT
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "save", SDL_SCANCODE_1
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "load", SDL_SCANCODE_2
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "deselect", SDL_SCANCODE_3
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "platform-path-create-mode-enter", SDL_SCANCODE_4
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "platform-path-select-mode-enter", SDL_SCANCODE_5
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "platform-path-node-append-mode-enter", SDL_SCANCODE_6
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "platform-path-node-select-mode-enter", SDL_SCANCODE_7
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "platform-path-node-move-mode-enter", SDL_SCANCODE_8
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "edit-angle-snap", SDL_SCANCODE_LSHIFT
    );
    Input_append_new_input_button_state_mapped_to_sdlScancode(
        editorInput, "delete", SDL_SCANCODE_DELETE
    );

    editorEntity.set<Input>(editorInput);
    editorEntity.set<MouseState>(mouseState);
    editorEntity.add<EditMode::PlatformPathSelectMode>();

    pinkGuyEntity.set<Input>(pinkGuyInput);

    SensorCollection pinkGuySensors;

    pinkGuySensors.sensor_rays[SENSOR_LEFT_FLOOR].position_start = (Position){-9.0f, 0.0f};
    pinkGuySensors.sensor_rays[SENSOR_LEFT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

    pinkGuySensors.sensor_rays[SENSOR_CENTER_FLOOR].position_start = (Position){0.0f, 0.0f};
    pinkGuySensors.sensor_rays[SENSOR_CENTER_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

    pinkGuySensors.sensor_rays[SENSOR_RIGHT_FLOOR].position_start = (Position){9.0f, 0.0f};
    pinkGuySensors.sensor_rays[SENSOR_RIGHT_FLOOR].distance = SENSOR_FLOOR_AIR_DISTANCE;

    pinkGuySensors.sensor_rays[SENSOR_LEFT_WALL].position_start = (Position){0.0f, 8.0f};
    pinkGuySensors.sensor_rays[SENSOR_LEFT_WALL].distance = SENSORS_DEFAULT_WALL_DISTANCE;

    pinkGuySensors.sensor_rays[SENSOR_RIGHT_WALL].position_start = (Position){0.0f, 8.0f};
    pinkGuySensors.sensor_rays[SENSOR_RIGHT_WALL].distance = SENSORS_DEFAULT_WALL_DISTANCE;




    StateCurrPrev state;
    state.currentState = STATE_ON_GROUND;
    state.prevState = STATE_ON_GROUND;

    pinkGuyEntity.set<AnimatedSprite>(animatedSprite);
    pinkGuyEntity.set<Position>((Position){640.0f/2.0f,480.0f/2.0f - 400});
    pinkGuyEntity.set<Velocity>((Velocity){0,0});
    pinkGuyEntity.set<Angle>((Angle){0.0f});
    pinkGuyEntity.set<StateCurrPrev>(state);
    pinkGuyEntity.set<SensorCollection>(pinkGuySensors);
    pinkGuyEntity.set<GroundSpeed>((GroundSpeed){0.0f});
    pinkGuyEntity.set<GroundMode>(GROUND_MODE_FLOOR);

    CollisionResultPlatformPathFloorSensor collision_result;
    collision_result.result.did_intersect = false;

    pinkGuyEntity.set<CollisionResultPlatformPathFloorSensor>(collision_result);

    gCameraPosition.x = pinkGuyEntity.get<Position>()->x;
    gCameraPosition.y = pinkGuyEntity.get<Position>()->y;
    
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
   

        // gCameraPosition.x = pinkGuyEntity.get<Position>()->x;
        // gCameraPosition.y = pinkGuyEntity.get<Position>()->y;


        world.progress();
    }
}