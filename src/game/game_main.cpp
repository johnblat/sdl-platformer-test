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


MouseState mouseState;

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

    ecs.system<Velocity, Input, StateCurrPrev, Angle>("keyStateVelocitySetter")
        .kind(flecs::PreUpdate)
        .iter(InputVelocitySetterSystem);

    ecs.system<Position, Sensors, Velocity, StateCurrPrev, Angle>("collision")
        .kind(flecs::PostUpdate)
        .iter(sensorsPvsCollisionSystem);

    ecs.system<Velocity, Position>("move")
        .kind(flecs::OnUpdate)
        .iter(moveSystem);

    ecs.system<AnimatedSprite, Input>()
        .kind(flecs::OnUpdate)
        .iter(InputFlipSystem);

    ecs.system<Input>()
        .kind(flecs::PreUpdate)
        .iter(inputUpdateSystem);

    ecs.system<Position, Sensors>()
        .kind(flecs::OnStore)
        .iter(renderSensorsSystem);

    ecs.system<AnimatedSprite, Velocity, StateCurrPrev>()
        .kind(flecs::OnUpdate)
        .iter(setAnimationBasedOnSpeedSystem);

    ecs.system<Velocity, StateCurrPrev>("gravity system")
        .kind(flecs::OnUpdate)
        .iter(gravitySystem);



    ecs.system<Position, PlatformVertices>()
        .kind(flecs::OnStore)
        .iter(renderPlatformVerticesSystem);

    ecs.system<Position, PlatformVertices>()
        .kind(flecs::OnStore)
        .iter(renderPlatformVerticesNodesSystem);

    ecs.system<>()
        .kind(flecs::OnStore)
        .iter(zoomRenderSetupSystem);

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(inputZoomSystem);

    ecs.system<Input>()
        .kind(flecs::OnStore)
        .iter(loadInputSystem);

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(DeselectInputSystem);

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




int main(){

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
    

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 0, walkAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 1, runAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 2, standingAttackAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 3, idleAnimation);
    
    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 4, jumpAnimation);


    
    Input pinkGuyInput;
    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("left"),
        SDL_SCANCODE_A,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });
    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("right"),
        SDL_SCANCODE_D,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });
    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("jump"),
        SDL_SCANCODE_SPACE,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });
    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("zoom-in"),
        SDL_SCANCODE_UP,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });
    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("zoom-out"),
        SDL_SCANCODE_DOWN,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });
    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("zoom-reset"),
        SDL_SCANCODE_R,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("save"),
        SDL_SCANCODE_1,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("load"),
        SDL_SCANCODE_2,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("deselect"),
        SDL_SCANCODE_SEMICOLON,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    pinkGuyInput.buttonStates.push_back((InputButtonState){
        std::string("edit-angle-snap"),
        SDL_SCANCODE_LSHIFT,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    pinkGuyEntity.set<Input>(pinkGuyInput);

    Sensors pinkGuySensors;
    pinkGuySensors.rays[LF_SENSOR].startingPosition = (Position){-8.0f, 0.0f};
    pinkGuySensors.rays[LF_SENSOR].distance = 16.0f;
    pinkGuySensors.rays[RF_SENSOR].startingPosition = (Position){8.0f, 0.0f};
    pinkGuySensors.rays[RF_SENSOR].distance = 16.0f;



    StateCurrPrev state;
    state.currentState = STATE_ON_GROUND;
    state.prevState = STATE_ON_GROUND;

    pinkGuyEntity.set<AnimatedSprite>(animatedSprite);
    pinkGuyEntity.set<Position>((Position){640.0f/2.0f,480.0f/2.0f - 400});
    pinkGuyEntity.set<Velocity>((Velocity){0,0});
    pinkGuyEntity.set<Angle>((Angle){0.0f});
    pinkGuyEntity.set<StateCurrPrev>(state);
    pinkGuyEntity.set<Sensors>(pinkGuySensors);


    
    registerSystems(world);
    
    
 

    PlatformVertices platformVertices;
    platformVertices.color.r = 255;
    platformVertices.color.g = 255;
    platformVertices.color.b = 255;

    platformVertices.vals.push_back((Position){-200.0f, -50.0f});
    platformVertices.vals.push_back((Position){200.0f, -50.0f});
    platformVertices.vals.push_back((Position){210.0f, -48.0f});
    platformVertices.vals.push_back((Position){220.0f, -46.0f});
    platformVertices.vals.push_back((Position){320.0f, -30.0f});
    platformVertices.vals.push_back((Position){640.0f, -40.0f});
    platformVertices.vals.push_back((Position){800.0f, -40.0f});
    platformVertices.vals.push_back((Position){1000.0f, -40.0f});


    flecs::entity platform = world.entity("platform");
    platform.set<Position>((Position){640.0f/2.0f, 480.0f/2.0f});
    platform.set<PlatformVertices>(platformVertices);

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

        mouseStateSetter(mouseState);
        EditPlatformVerticesAddVertexAtMousePositionOnSelected(world, mouseState);

        world.progress();
    }
}