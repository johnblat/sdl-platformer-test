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


SDL_Renderer *gRenderer;
SDL_Window *gWindow;

int gScreenWidth = 640 * 2;
int gScreenHeight = 480 * 2;

SpriteSheet gSpriteSheets[MAX_SPRITE_SHEETS]; 
size_t gNumSpriteSheets = 0;


typedef struct Dimensions Dimensions;
struct Dimensions {
    float w, h;
};


void registerSystems(flecs::world &ecs){
// Set up the animation playing and rendering systems
    ecs.system<AnimatedSprite>("AnimatedSpritePlay").kind(flecs::OnUpdate).iter(animationsAccumulationSystem);

    ecs.system<AnimatedSprite, Position>("renderingAnimatedSprites").kind(flecs::OnStore).iter(renderingAnimatedSpritesSystem);

    ecs.system<AnimatedSprite, KeyboardState>("keyStateAnimationSpriteState").kind(flecs::OnUpdate).iter(KeyboardStateAnimationSetterSystem);

    ecs.system<Velocity, Input, StateCurrPrev, Angle>("keyStateVelocitySetter").kind(flecs::PreUpdate).iter(InputVelocitySetterSystem);

    ecs.system<Position, std::vector<Ray2d>, Velocity, StateCurrPrev, Angle>("collision").kind(flecs::PostUpdate).iter(
            ray2dSolidRectCollisionSystem);

    ecs.system<Velocity, Position>("move").kind(flecs::OnUpdate).iter(moveSystem);

    ecs.system<AnimatedSprite, Input>().kind(flecs::OnUpdate).iter(InputFlipSystem);

    ecs.system<Input>().kind(flecs::PreUpdate).iter(inputUpdateSystem);

    ecs.system<Position, std::vector<Ray2d>>().kind(flecs::OnStore).iter(renderRay2dCollectionsSystem);

    ecs.system<AnimatedSprite, Velocity, StateCurrPrev>().kind(flecs::OnUpdate).iter(setAnimationBasedOnSpeedSystem);

    ecs.system<Velocity, StateCurrPrev>("gravity system").kind(flecs::OnUpdate).iter(gravitySystem);

    ecs.system<Position, SolidRect>().kind(flecs::OnStore).iter(renderRectangularObjectsSystem);

    ecs.system<Position, PlatformVertices>().kind(flecs::OnStore).iter(renderPlatformVerticesSystem);
}





int main(){
    bool quit = false;
    /**
     * FLECS SETUP
     * 
     */

    flecs::world world;
    flecs::entity pinkGuyEntity = world.entity("PinkGuy");
    flecs::entity owlGuyEntity = world.entity("OwlGuy");
    flecs::entity floor1Entity = world.entity("Floor");
    flecs::entity floor2Entity = world.entity("Floor2");


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
    SDL_Surface *bgSurface = IMG_Load("res/checkerboard-bg.png");
    SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(gRenderer, bgSurface);
    float parallaxBgScale = 0.1;
    int bg_w = bgSurface->w;
    int bg_h = bgSurface->h;
    Position bgPosition = {gScreenWidth/2.0f,gScreenHeight/2.0f};
    SDL_Rect bgDestRect = {(int)bgPosition.x - bg_w/2,(int) bgPosition.y - bg_h/2, bg_w, bg_h}; 
    SDL_FreeSurface(bgSurface);


    /**
     * ANIMATED SPRITE SETUP
     * 
     */
    pinkGuyEntity.add<AnimatedSprite>();

    const char *filename = "res/pink-monster-animation-transparent.png";
    const char *animatedSpriteName =  "pink-monster-animation";
    u32 spriteSheetId = createSpriteSheet(filename, 9, 15, animatedSpriteName);
    AnimatedSprite animatedSprite = createAnimatedSprite(spriteSheetId);

    // add animations to animatedSprite
    Animation walkAnimation{};
    {
        walkAnimation.accumulator = 0.0f;
        walkAnimation.arrFrames[0] = 15;
        walkAnimation.arrFrames[1] = 16;
        walkAnimation.arrFrames[2] = 17;
        walkAnimation.arrFrames[3] = 18;
        walkAnimation.arrFrames[4] = 19;
        walkAnimation.arrFrames[5] = 20;
        walkAnimation.numFrames = 6;
        walkAnimation.currentFrame = 0;
        walkAnimation.fps = 12;
        walkAnimation.msPerFrame = 0.0833;
        walkAnimation.isLoop = true;

        const char *walkAnimationName = "walk";
        strncpy(walkAnimation.name, walkAnimationName, 5);
    }


    Animation runAnimation{};
    {
        runAnimation.accumulator = 0.0f;
        runAnimation.arrFrames[0] = 30;
        runAnimation.arrFrames[1] = 31;
        runAnimation.arrFrames[2] = 32;
        runAnimation.arrFrames[3] = 33;
        runAnimation.arrFrames[4] = 34;
        runAnimation.arrFrames[5] = 35;
        runAnimation.numFrames = 6;
        runAnimation.currentFrame = 0;
        runAnimation.fps = 12;
        runAnimation.msPerFrame = 0.0833;
        runAnimation.isLoop = true;

        const char *runAnimationName = "run";
        strncpy(runAnimation.name, runAnimationName, 4);
    }
    

    Animation standingAttackAnimation{};
    {
        standingAttackAnimation.accumulator = 0.0f;
        standingAttackAnimation.arrFrames[0] = 45;
        standingAttackAnimation.arrFrames[1] = 46;
        standingAttackAnimation.arrFrames[2] = 47;
        standingAttackAnimation.arrFrames[3] = 48;
        standingAttackAnimation.numFrames = 4;
        standingAttackAnimation.currentFrame = 0;
        standingAttackAnimation.fps = 12;
        standingAttackAnimation.msPerFrame = 0.0833;
        standingAttackAnimation.isLoop = false;
        const char *standingAttackAnimationName = "stand-attack";
        strncpy(standingAttackAnimation.name, standingAttackAnimationName, 16);
    }

    Animation idleAnimation{};
    {

        idleAnimation.accumulator = 0.0f;
        idleAnimation.arrFrames[0] = 75;
        idleAnimation.arrFrames[1] = 76;
        idleAnimation.arrFrames[2] = 77;
        idleAnimation.arrFrames[3] = 78;
        idleAnimation.arrFrames[4] = 77;
        idleAnimation.arrFrames[5] = 76;

        idleAnimation.numFrames = 6;
        idleAnimation.currentFrame = 0;
        idleAnimation.fps = 12;
        idleAnimation.msPerFrame = 0.0833;
        idleAnimation.isLoop = true;
        const char *idleAnimationName = "idle";
        strncpy(idleAnimation.name, idleAnimationName, 16);
    }
    
    Animation jumpAnimation{};
    {
        jumpAnimation.accumulator = 0.0f;
        jumpAnimation.arrFrames[0] = 120;
        jumpAnimation.arrFrames[1] = 121;
        jumpAnimation.arrFrames[2] = 122;
        jumpAnimation.arrFrames[3] = 123;
        jumpAnimation.arrFrames[4] = 124;
        jumpAnimation.arrFrames[5] = 125;



        jumpAnimation.numFrames = 6;
        jumpAnimation.currentFrame = 0;
        jumpAnimation.fps = 12;
        jumpAnimation.msPerFrame = 0.0833;
        jumpAnimation.isLoop = true;
        const char *jumpAnimationName = "jump";
        strncpy(jumpAnimation.name, jumpAnimationName, 16);
    }

    Animation fallAnimation{};
    {
        fallAnimation.accumulator = 0.0f;
        fallAnimation.arrFrames[0] = 26;
        fallAnimation.arrFrames[1] = 27;
        fallAnimation.arrFrames[2] = 28;
        fallAnimation.arrFrames[3] = 29;

        fallAnimation.numFrames = 4;
        fallAnimation.currentFrame = 0;
        fallAnimation.fps = 12;
        fallAnimation.msPerFrame = 0.0833;
        fallAnimation.isLoop = false;
        const char *fallAnimationName = "fall";
        strncpy(fallAnimation.name, fallAnimationName, 16);
    }

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

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 5, fallAnimation);

    animatedSprite.currentAnimation = 0;
    
    // ADD the setup AnimatedSprite to the pinkGuyEntity
    pinkGuyEntity.set<AnimatedSprite>(animatedSprite);
    pinkGuyEntity.set<Position>((Position){640.0f/2.0f,480.0f/2.0f - 400});
    pinkGuyEntity.set<Velocity>((Velocity){0,0});
    pinkGuyEntity.set<Angle>((Angle){0.0f});
    // pinkGuyEntity.set<CollisionRect>((CollisionRect){32,32});
    
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


    pinkGuyEntity.set<Input>(pinkGuyInput);

    Dimensions pinkDimensions;
    pinkDimensions.h = 32;
    pinkGuyEntity.set<Dimensions>(pinkDimensions);
     
    std::vector<Ray2d> rays;
    Ray2d ray0, ray1;
    ray0.startingPosition.x = -8.0f;
    ray0.startingPosition.y = 0.0f;
    ray0.distance = 16.0f;
    ray1.startingPosition.x = 8.0f;
    ray1.startingPosition.y = 0.0f;
    ray1.distance = 16.0f;
    rays.push_back(ray0);
    
    rays.push_back(ray1);

    pinkGuyEntity.set<std::vector<Ray2d>>(rays);

    StateCurrPrev state;
    state.currentState = STATE_ON_GROUND;
    state.prevState = STATE_ON_GROUND;

    pinkGuyEntity.set<StateCurrPrev>(state);
    

    // OTHER CHARACTER SETUP

    
    registerSystems(world);
    
    
 

    PlatformVertices platformVertices;
    platformVertices.color.r = 255;
    platformVertices.color.g = 255;
    platformVertices.color.b = 255;

    platformVertices.vals.push_back((PlatformVertex){-200.0f, -50.0f});
    platformVertices.vals.push_back((PlatformVertex){200.0f, -50.0f});
    platformVertices.vals.push_back((PlatformVertex){210.0f, -48.0f});
    platformVertices.vals.push_back((PlatformVertex){220.0f, -46.0f});
    platformVertices.vals.push_back((PlatformVertex){320.0f, -30.0f});
    platformVertices.vals.push_back((PlatformVertex){640.0f, -40.0f});
    platformVertices.vals.push_back((PlatformVertex){800.0f, -40.0f});
    platformVertices.vals.push_back((PlatformVertex){1000.0f, -40.0f});





    flecs::entity platform = world.entity("platform");
    platform.set<Position>((Position){640.0f/2.0f, 480.0f/2.0f});
    platform.set<PlatformVertices>(platformVertices);

    loadPlatformVertices(world);



    // timing
    // float deltaTime = 0.0f;
    const float FPS = 60;
    const float secondsPerFrame = 1.0f / FPS;

    // main loop
    while(!quit){
        u64 startTicks = SDL_GetTicks();
        // deltaTime = getDeltaTime();

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
                break;
            }
        }
        u8 *keyStates = (u8 *)SDL_GetKeyboardState(nullptr);
        KeyboardState keyboardState{};
        keyboardState.keyStates = keyStates;    

        gKeyStates = keyStates;
   


        pinkGuyEntity.set<KeyboardState>(keyboardState);

        SDL_SetRenderDrawColor(gRenderer, 0,0,0,255);
        SDL_RenderClear(gRenderer);

        gCameraPosition.x = pinkGuyEntity.get<Position>()->x;
        gCameraPosition.y = pinkGuyEntity.get<Position>()->y;

        Position centerScreen = {(float)gScreenWidth/2, (float)gScreenHeight/2};
        Position scaledCenterScreen = {centerScreen.x / gZoomAmount, centerScreen.y / gZoomAmount};
    
        bgDestRect.x =
                (((int) bgPosition.x - (int) gCameraPosition.x*parallaxBgScale) + (int) scaledCenterScreen.x -
                ((int) bg_w / 2));
        bgDestRect.y =
                ((int) bgPosition.y - (int) gCameraPosition.y * parallaxBgScale + (int) scaledCenterScreen.y - ((int) bg_h / 2) );

        SDL_RenderSetScale(gRenderer, gZoomAmount, gZoomAmount);

        // draw background
        SDL_RenderCopy(gRenderer, bgTexture, nullptr, &bgDestRect);

        world.progress();


        SDL_RenderPresent(gRenderer);

        u64 endTicks = SDL_GetTicks();

        u64 totalTicks = endTicks - startTicks;
        float totalSeconds = (float)totalTicks / 1000.0f;

        if(totalSeconds < secondsPerFrame){
            float secondsRemainingToFixTimeStep = secondsPerFrame - totalSeconds;
            float msRemainingToFixTimeStep = secondsRemainingToFixTimeStep * 1000;
            SDL_Delay((u32)msRemainingToFixTimeStep);
            //printf("ms to wait: %f\n", msRemainingToFixTimeStep);
        }
    }


    
}