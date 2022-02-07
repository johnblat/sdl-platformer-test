#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "animation.h"
#include "animationProcessing.h"
#include "jbInts.h"
#include "render.h"
#include "spriteSheets.h"
#include "spriteSheetsProcessing.h"
#include "window.h"
#include "string.h"
#include "timestep.h"
#include "velocity.h"
#include "movement.h"
#include "input.h"
#include "flecs.h"
#include <string>
#include <vector>


SDL_Renderer *gRenderer;
SDL_Window *gWindow;

SpriteSheet gSpriteSheets[MAX_SPRITE_SHEETS]; 
size_t gNumSpriteSheets = 0;

typedef struct RectangularObject RectangularObject;
struct RectangularObject {
    SDL_Rect rect;
    SDL_Color color;
};


// always points downwards
typedef struct Ray2d Ray2d;
struct Ray2d {
    Position startingPosition;
    float distance; 
};

typedef struct PhysicsData PhysicsData;
struct PhysicsData {
    bool onGround;
};

typedef struct Ray2dCollection Ray2dCollection;
struct Ray2dCollection {
    std::vector<Ray2d> rays;
};

struct Sensors {
    Position bottomLeftAnchorPoint;
    Position bottomRightAnchorPoint;
};

void renderRectangularObjectsSystem(flecs::iter &it, RectangularObject *rectObjects){
    // FIX THISS
    Position centerScreen = {320, 240};


    for(auto i : it){
        SDL_SetRenderDrawColor(
            gRenderer, 
            rectObjects[i].color.r, 
            rectObjects[i].color.g, 
            rectObjects[i].color.b, 
            255
        );
        float scale;
        SDL_RenderGetScale(gRenderer,&scale, NULL );
        Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};

        SDL_Rect renderRect;
        renderRect.x = rectObjects[i].rect.x - gCameraPosition.x + scaledCenterScreen.x;
        renderRect.y = rectObjects[i].rect.y - gCameraPosition.y + scaledCenterScreen.y;
        renderRect.w = rectObjects[i].rect.w;
        renderRect.h = rectObjects[i].rect.h;

        SDL_RenderFillRect(gRenderer, &renderRect);
    }
}


void ray2dRectangularObjectCollisionSystem(flecs::iter &it, Position *positions, std::vector<Ray2d> *ray2dCollections ){
    for(int i : it){
        // check against rectangular objects
        Ray2d rayLocal = ray2dCollections[i].at(0);
        Ray2d rayGlobal;
        rayGlobal.startingPosition.x = positions[i].x + rayLocal.startingPosition.x;
        rayGlobal.startingPosition.y = positions[i].y + rayLocal.startingPosition.y;

        auto f = it.world().filter<RectangularObject>();
        f.each([&](flecs::entity e, RectangularObject &rectObj){
            if(rayGlobal.startingPosition.x > rectObj.rect.x
            && rayGlobal.startingPosition.x < rectObj.rect.x + rectObj.rect.w){
                //ray x is in the rect
                if(rayGlobal.startingPosition.y > rectObj.rect.y
                && rayGlobal.startingPosition.y < rectObj.rect.y + rectObj.rect.h){
                    // ray y is inside rect
                    positions[i].y = rectObj.rect.y - rayLocal.distance;
                }
                else if(rayGlobal.startingPosition.y + rayLocal.distance > rectObj.rect.y
                && rayGlobal.startingPosition.y + rayLocal.distance < rectObj.rect.y + rectObj.rect.h){
                    positions[i].y = rectObj.rect.y - rayLocal.distance;
                }
            }
        });
    }
}



void renderRay2dCollectionsSystem(flecs::iter &it, Position *positions, std::vector<Ray2d> *ray2dCollections){
    // move this somewhere else
    Position centerScreen = {320, 240};

    for(int i : it){
        std::vector<Ray2d> ray2ds = ray2dCollections[i];
        for(int vi = 0; vi < ray2ds.size(); vi++ ){
            float scale;
            SDL_RenderGetScale(gRenderer,&scale, NULL );
            Position scaledCenterScreen = {centerScreen.x / scale, centerScreen.y / scale};

            Ray2d ray = ray2ds[vi];
            Position actualPosition; 
            actualPosition.x = positions[i].x + ray.startingPosition.x;
            actualPosition.y = positions[i].y + ray.startingPosition.y;
            actualPosition.x = actualPosition.x - gCameraPosition.x + scaledCenterScreen.x;
            actualPosition.y = actualPosition.y - gCameraPosition.y + scaledCenterScreen.y;

            SDL_SetRenderDrawColor(gRenderer, 255,0,0,255);
            SDL_RenderDrawLine(gRenderer, actualPosition.x, actualPosition.y, actualPosition.x, actualPosition.y + ray.distance);

            SDL_SetRenderDrawColor(gRenderer, 0,0,0,255);
            SDL_RenderDrawLine(gRenderer, actualPosition.x - 1, actualPosition.y, actualPosition.x - 1, actualPosition.y + ray.distance);
            SDL_RenderDrawLine(gRenderer, actualPosition.x + 1, actualPosition.y, actualPosition.x + 1, actualPosition.y + ray.distance);


        }
    }
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
    gWindow = SDL_CreateWindow("title",0,0, 640,480, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    SDL_Event event;

    /**
     * SETUP BACKGROUND
     * 
     */
    SDL_Surface *bgSurface = IMG_Load("bg.png");
    SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(gRenderer, bgSurface);
    float parralaxBgScale = 0.25;
    int bg_w = bgSurface->w;
    int bg_h = bgSurface->h;
    Position bgPosition = {640/2,480/2};
    SDL_Rect bgDestRect = {(int)bgPosition.x - bg_w/2,(int) bgPosition.y - bg_h/2, bg_w, bg_h}; 
    SDL_FreeSurface(bgSurface);


    /**
     * ANIMATED SPRITE SETUP
     * 
     */
    pinkGuyEntity.add<AnimatedSprite>();

    const char *filename = "pink-monster-animation-transparent.png";
    const char *animatedSpriteName =  "pink-monster-animation";
    u32 spriteSheetId = createSpriteSheet(filename, 15, 8, animatedSpriteName);
    AnimatedSprite animatedSprite = createAnimatedSprite(spriteSheetId);

    // add animations to animatedSprite
    Animation walkAnimation;
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


    Animation runAnimation;
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
    

    Animation standingAttackAnimation;
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

    Animation idleAnimation;
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
    
    
    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 0, walkAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 1, runAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 2, standingAttackAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 3, idleAnimation);

    animatedSprite.currentAnimation = 0;
    
    // ADD the setup AnimatedSprite to the pinkGuyEntity
    pinkGuyEntity.set<AnimatedSprite>(animatedSprite);
    pinkGuyEntity.set<Position>((Position){640/2,480/2});
    pinkGuyEntity.set<Velocity>((Velocity){0,0});
    // pinkGuyEntity.set<CollisionRect>((CollisionRect){32,32});
    
    InputButtonState bstates[3];
    bstates[0].currentInputState = INPUT_IS_NOT_PRESSED;
    bstates[0].previousInputState = INPUT_IS_NOT_PRESSED;
    bstates[0].name = std::string("left");
    bstates[0].sdlScancode = SDL_SCANCODE_A;

    bstates[1].currentInputState = INPUT_IS_NOT_PRESSED;
    bstates[1].previousInputState = INPUT_IS_NOT_PRESSED;
    bstates[1].name = "right";
    bstates[1].sdlScancode = SDL_SCANCODE_D;

    bstates[2].currentInputState = INPUT_IS_NOT_PRESSED;
    bstates[2].previousInputState = INPUT_IS_NOT_PRESSED;
    bstates[2].name = "jump";
    bstates[2].sdlScancode = SDL_SCANCODE_SPACE;

    Input pinkGuyInput;
    pinkGuyInput.buttonStates = bstates;
    pinkGuyInput.numButtomStates = 3;

    pinkGuyEntity.set<Input>(pinkGuyInput);

    std::vector<Ray2d> rays;
    Ray2d ray0;
    ray0.startingPosition.x = 0.0f;
    ray0.startingPosition.y = 0.0f;
    ray0.distance = 16.0f;
    rays.push_back(ray0);

    pinkGuyEntity.set<std::vector<Ray2d>>(rays);


    

    // OTHER CHARACTER SETUP

    owlGuyEntity.add<AnimatedSprite>();

    const char *filename2 = "owlet-monster-animation-transparent.png";
    const char *animatedSpriteName2 =  "owl-monster-animation";
    u32 spriteSheetId2 = createSpriteSheet(filename2, 15, 8, animatedSpriteName2);
    AnimatedSprite animatedSprite2 = createAnimatedSprite(spriteSheetId2);

    // add animations to animatedSprite
    Animation walkAnimation2;
    {
        walkAnimation2.accumulator = 0.0f;
        walkAnimation2.arrFrames[0] = 15;
        walkAnimation2.arrFrames[1] = 16;
        walkAnimation2.arrFrames[2] = 17;
        walkAnimation2.arrFrames[3] = 18;
        walkAnimation2.arrFrames[4] = 19;
        walkAnimation2.arrFrames[5] = 20;
        walkAnimation2.numFrames = 6;
        walkAnimation2.currentFrame = 0;
        walkAnimation2.fps = 12;
        walkAnimation2.msPerFrame = 0.0833;
        walkAnimation2.isLoop = true;
    }


    const char *walkAnimation2Name = "walk";
    strncpy(walkAnimation2.name, walkAnimation2Name, 5);

    

    Animation runAnimation2;
    {
        runAnimation2.accumulator = 0.0f;
        runAnimation2.arrFrames[0] = 30;
        runAnimation2.arrFrames[1] = 31;
        runAnimation2.arrFrames[2] = 32;
        runAnimation2.arrFrames[3] = 33;
        runAnimation2.arrFrames[4] = 34;
        runAnimation2.arrFrames[5] = 35;
        runAnimation2.numFrames = 6;
        runAnimation2.currentFrame = 0;
        runAnimation2.fps = 12;
        runAnimation2.msPerFrame = 0.0833;
        runAnimation2.isLoop = true;
    }


    const char *runAnimation2Name = "run";
    strncpy(runAnimation2.name, runAnimation2Name, 3);

    Animation standingAttackAnimation2;
    {
        standingAttackAnimation2.accumulator = 0.0f;
        standingAttackAnimation2.arrFrames[0] = 45;
        standingAttackAnimation2.arrFrames[1] = 46;
        standingAttackAnimation2.arrFrames[2] = 47;
        standingAttackAnimation2.arrFrames[3] = 48;
        standingAttackAnimation2.numFrames = 4;
        standingAttackAnimation2.currentFrame = 0;
        standingAttackAnimation2.fps = 12;
        standingAttackAnimation2.msPerFrame = 0.0833;
        standingAttackAnimation2.isLoop = false;
        const char *standingAttackAnimation2Name = "stand-attack";
        strncpy(standingAttackAnimation2.name, standingAttackAnimation2Name, 16);
    }

    
    addNewAnimationToAnimatedSprite(&animatedSprite2);
    overwriteAnimationOnAnimatedSprite(&animatedSprite2, 0, walkAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite2);
    overwriteAnimationOnAnimatedSprite(&animatedSprite2, 1, runAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite2);
    overwriteAnimationOnAnimatedSprite(&animatedSprite2, 2, standingAttackAnimation);

    animatedSprite2.currentAnimation = 0;

    owlGuyEntity.set<AnimatedSprite>(animatedSprite2);
    owlGuyEntity.set<Position>((Position){640/2 + 50,480/2});


    
    
    // Set up the animation playing and rendering systems
    world.system<AnimatedSprite>("AnimatedSpritePlay").kind(flecs::OnUpdate).iter(animationsAccumulationSystem);

    world.system<AnimatedSprite, Position>("renderingAnimatedSprites").kind(flecs::OnStore).iter(renderingAnimatedSpritesSystem);

    world.system<AnimatedSprite, KeyboardState>("keyStateAnimationSpriteState").kind(flecs::OnUpdate).iter(KeyboardStateAnimationSetterSystem);

    world.system<Velocity, Input>("keyStateVelocitySetter").kind(flecs::OnUpdate).iter(InputVelocitySetterSystem);
    world.system<Position, std::vector<Ray2d>>("collision").kind(flecs::PostUpdate).iter(ray2dRectangularObjectCollisionSystem);

    world.system<Velocity, Position>("move").kind(flecs::OnUpdate).iter(moveSystem);

    world.system<AnimatedSprite, Input>().kind(flecs::OnUpdate).iter(InputFlipSystem);

    world.system<Input>().kind(flecs::PreUpdate).iter(inputUpdateSystem);

    world.system<Position, std::vector<Ray2d>>().kind(flecs::OnStore).iter(renderRay2dCollectionsSystem);

    world.system<AnimatedSprite, Velocity>().kind(flecs::OnUpdate).iter(setAnimationBasedOnSpeedSystem);

    // TEST Rectangular objects

    SDL_Rect floorRect = {0,300,2000,40};
    SDL_Color floorRectColor = {0,0,200};

    RectangularObject robj;
    robj.rect = floorRect;
    robj.color = floorRectColor;

    SDL_Rect floorRect2 = {500,290,2000,40};
    SDL_Color floorRectColor2 = {0,0,200};

    RectangularObject robj2;
    robj2.rect = floorRect2;
    robj2.color = floorRectColor2;

    world.system<RectangularObject>().kind(flecs::OnStore).iter(renderRectangularObjectsSystem);

    floor1Entity.set<RectangularObject>(robj); 
    floor2Entity.set<RectangularObject>(robj2); 


    // timing
    // float deltaTime = 0.0f;
    const float FPS = 60;
    const float secondsPerFrame = 1.0f / FPS;

    float zoomAmount = 1.0f;
    // main loop
    while(!quit){
        u64 startTicks = SDL_GetTicks();
        // deltaTime = getDeltaTime();

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;\
                break;
            }
        }
        u8 *keyStates = (u8 *)SDL_GetKeyboardState(NULL);
        KeyboardState keyboardState;
        keyboardState.keyStates = keyStates;    

        gKeyStates = keyStates;

        if(keyStates[SDL_SCANCODE_UP]){
            zoomAmount += 0.005;
        }       

        if(keyStates[SDL_SCANCODE_DOWN]){
            zoomAmount -= 0.005;
        }       

        if(keyStates[SDL_SCANCODE_R]){
            zoomAmount = 1.0;
        }       


        pinkGuyEntity.set<KeyboardState>(keyboardState);

        SDL_SetRenderDrawColor(gRenderer, 0,0,0,255);
        SDL_RenderClear(gRenderer);

        gCameraPosition.x = pinkGuyEntity.get<Position>()->x;
        gCameraPosition.y = pinkGuyEntity.get<Position>()->y;

        Position centerScreen = {320, 240};
        Position scaledCenterScreen = {centerScreen.x / zoomAmount, centerScreen.y / zoomAmount};
        float scaledParallaxScale = parralaxBgScale / zoomAmount; 
        // background moving with camera
        bgDestRect.x = (((bgPosition.x - gCameraPosition.x)* parralaxBgScale + scaledCenterScreen.x) - (bg_w/2) ) ;
        bgDestRect.y = ((bgPosition.y - gCameraPosition.y + scaledCenterScreen.y) - (bg_h / 2) );

        SDL_RenderSetScale(gRenderer, zoomAmount, zoomAmount);

        // draw background
        SDL_RenderCopy(gRenderer, bgTexture, NULL, &bgDestRect);

        world.progress();
        // animationsAccumulationSystem(&animatedSprite, 1, deltaTime);


        // renderingAnimatedSpritesSystem(&animatedSprite, &position, 1);

        //renderAnimatedSprite(640 / 2, 480 / 2, animatedSprite);

        SDL_RenderPresent(gRenderer);

        u64 endTicks = SDL_GetTicks();

        u64 totalTicks = endTicks - startTicks;
        float totalSeconds = (float)totalTicks / 1000.0f;

        if(totalSeconds < secondsPerFrame){
            float secondsRemainingToFixTimeStep = secondsPerFrame - totalSeconds;
            float msRemainingToFixTimeStep = secondsRemainingToFixTimeStep * 1000;
            SDL_Delay(msRemainingToFixTimeStep);
            // printf("ms to wait: %f\n", msRemainingToFixTimeStep);
        }
    }


    
}