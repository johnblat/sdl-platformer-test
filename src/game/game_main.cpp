#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <vector>
#include "animation.h"
#include "animationProcessing.h"
#include "ints.h"
#include "spriteSheets.h"
#include "spriteSheetsProcessing.h"
#include "velocity.h"
#include "movement.h"
#include "input.h"
#include "flecs.h"
#include "ray2d.h"
#include "states.h"
#include "solid_rect.h"
#include "collisions.h"
#include "debug_display.h"
#include "resourceLoading.h"
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

int gScreenWidth = 640 * 2;
int gScreenHeight = 480 * 2;

SpriteSheet gSpriteSheets[MAX_SPRITE_SHEETS]; 
size_t gNumSpriteSheets = 0;



void registerSystems(flecs::world &ecs){
// Set up the animation playing and rendering systems
    ecs.system<AnimatedSprite>("AnimatedSpritePlay")
        .kind(flecs::OnUpdate)
        .iter(animationsAccumulationSystem);

    ecs.system<AnimatedSprite, Position, Angle>("renderingAnimatedSprites")
        .kind(flecs::OnStore)
        .iter(renderingAnimatedSpritesSystem);

    ecs.system<AnimatedSprite, KeyboardState>("keyStateAnimationSpriteState")
        .kind(flecs::OnUpdate)
        .iter(KeyboardStateAnimationSetterSystem);

    ecs.system<Velocity, GroundSpeed, Input, StateCurrPrev, Angle>("keyStateVelocitySetter")
        .kind(flecs::PreUpdate)
        .iter(InputVelocitySetterSystem);

    ecs.system<Position, Sensors, Velocity, GroundSpeed, GroundMode, StateCurrPrev, Angle>("collision")
        .kind(flecs::PostUpdate)
        .iter(sensorsPVCsCollisionSystem);

    ecs.system<Velocity, Position>("move")
        .kind(flecs::OnUpdate)
        .iter(moveSystem);

    ecs.system<AnimatedSprite, Input>()
        .kind(flecs::OnUpdate)
        .iter(InputFlipSystem);

    ecs.system<Input>()
        .kind(flecs::PreUpdate)
        .iter(inputUpdateSystem);
    
    ecs.system<MouseState>()
        .kind(flecs::PreUpdate)
        .iter(mouseStateSetterSystem);

    ecs.system<Position, Sensors, GroundMode>()
        .kind(flecs::OnStore)
        .iter(renderSensorsSystem);

    ecs.system<AnimatedSprite, Velocity, StateCurrPrev>()
        .kind(flecs::OnUpdate)
        .iter(setAnimationBasedOnSpeedSystem);

    ecs.system<Velocity, StateCurrPrev>("gravity system")
        .kind(flecs::OnUpdate)
        .iter(gravitySystem);

    ecs.system<Position, PlatformVertexCollection>()
        .kind(flecs::OnStore)
        .iter(renderPlatformVerticesSystem);

    ecs.system<Position, PlatformVertexCollection>()
        .kind(flecs::OnStore)
        .iter(renderPlatformVerticesNodesSystem);

    ecs.system<>()
        .kind(flecs::OnStore)
        .iter(zoomRenderSetupSystem);

    ecs.system<Input, MouseState>()
        .kind(flecs::OnStore)
        .iter(renderUncommitedLinesToPlaceSystem);

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(inputZoomSystem);

    ecs.system<Input>()
        .kind(flecs::OnStore)
        .iter(loadInputSystem);

    ecs.system<Input>()
        .kind(flecs::OnStore)
        .iter(saveSystem);

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(DeselectInputSystem);
    
    ecs.system<Input, MouseState>()
        .kind(flecs::OnUpdate)
        .iter(EditPlatformVerticesAddVertexAtMousePositionOnSelectedSystem);
        
    ecs.system<Position, PlatformVertexCollection, SelectedForEditing>()
        .kind(flecs::OnUpdate)
        .iter(SelectedPlatformVertexCollectionDeletionSystem);

    ecs.system<>()
        .kind(flecs::PreFrame)
        .iter(renderFrameStartSystem);

    ecs.system<Position, ParallaxSprite>()
        .kind(flecs::OnUpdate)
        .iter(renderParallaxSpriteSystem);

    ecs.system<>()
        .kind(flecs::PostFrame)
        .iter(renderEndFrameSystem);
}

void registerObservers(flecs::world &ecs){
    ecs.observer<PlatformVertexCollection>("OnSelect")
        .event(flecs::OnAdd)
        .term<SelectedForEditing>()
        .iter(setColorOnPVCSelect);

    ecs.observer<PlatformVertexCollection>("OnDeselect")
        .event(flecs::OnRemove)
        .term<SelectedForEditing>()
        .iter(setColorOnPVCDeselect);

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
    u32 bgSpriteId = createSpriteSheet(
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

    flecs::entity pxBgEntity = world.entity();
    pxBgEntity.set<Position>(parallaxSpritePosition);
    pxBgEntity.set<ParallaxSprite>(parallaxSprite);




    /**
     * ANIMATED SPRITE SETUP
     * 
     */
     pinkGuyEntity.add<AnimatedSprite>();

    std::string filename = "res/pink-monster-animation-transparent.png";
    std::string animatedSpriteName =  "pink-monster-animation";
    u32 spriteSheetId = createSpriteSheet(filename, 9, 15, animatedSpriteName);
    AnimatedSprite animatedSprite = createAnimatedSprite(spriteSheetId);

    Animation walkAnimation = createAnimation({15,16,17,18,19,20}, 12.0f, true, "walk");
    Animation runAnimation = createAnimation({30,31,32,33,34,35}, 12.0f, true, "run");
    Animation standingAttackAnimation = createAnimation({45,46,47,48}, 12.0f, false, "stand-attack");
    Animation idleAnimation = createAnimation({75,76,77,78,77,76}, 12.0f, true, "idle");
    Animation jumpAnimation = createAnimation({120,121,122,123,124,125}, 12.0f, true, "jump");
    

    addAnimationToAnimatedSprite(animatedSprite, walkAnimation);
    addAnimationToAnimatedSprite(animatedSprite, runAnimation);
    addAnimationToAnimatedSprite(animatedSprite, idleAnimation);
    addAnimationToAnimatedSprite(animatedSprite, standingAttackAnimation);
    addAnimationToAnimatedSprite(animatedSprite, jumpAnimation);


    
    Input pinkGuyInput;
    addButtonToInput(pinkGuyInput, "left", SDL_SCANCODE_A);
    addButtonToInput(pinkGuyInput, "right", SDL_SCANCODE_D);
    addButtonToInput(pinkGuyInput, "jump", SDL_SCANCODE_SPACE);
    addButtonToInput(pinkGuyInput, "zoom-in", SDL_SCANCODE_UP);
    addButtonToInput(pinkGuyInput, "zoom-out", SDL_SCANCODE_DOWN);
    addButtonToInput(pinkGuyInput, "zoom-reset", SDL_SCANCODE_R);
    addButtonToInput(pinkGuyInput, "save", SDL_SCANCODE_1);
    addButtonToInput(pinkGuyInput, "load", SDL_SCANCODE_2);
    addButtonToInput(pinkGuyInput, "deselect", SDL_SCANCODE_LCTRL);
    addButtonToInput(pinkGuyInput, "edit-angle-snap", SDL_SCANCODE_LSHIFT);
    addButtonToInput(pinkGuyInput, "delete", SDL_SCANCODE_DELETE);


    pinkGuyEntity.set<Input>(pinkGuyInput);
    pinkGuyEntity.set<MouseState>(mouseState);

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
    registerObservers(world);

    
    loadPlatformVertices(world);

    gTimeStep = TimeStepInit(60.0f);

    // main loop
    while(!quit){

        TimeStepSetStartTicks(gTimeStep);

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