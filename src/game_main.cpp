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

#include "flecs.h"

SDL_Renderer *gRenderer;
SDL_Window *gWindow;

SpriteSheet gSpriteSheets[MAX_SPRITE_SHEETS]; 
size_t gNumSpriteSheets = 0;


struct KeyMap {
    char name[16];
    SDL_Scancode sdlScancode;
};

struct KeyboardState{
    u8 *keyStates;
};

void keyStateAnimationSetterSystem(flecs::iter &it, AnimatedSprite *animatedSprites, KeyboardState *keyStatesCollections){
    for(auto i : it){
        u8 *keyStates = keyStatesCollections[i].keyStates;
        if(keyStates[SDL_SCANCODE_0]){
            animatedSprites[i].currentAnimation = 0;
        } 
        if(keyStates[SDL_SCANCODE_1]){
            animatedSprites[i].currentAnimation = 1;
            // AnimatedSprites[i] *sprite = pinkGuyEntity.get<AnimatedSprites[i]>();

        } 
        if(keyStates[SDL_SCANCODE_2]){
            animatedSprites[i].currentAnimation = 2;
            restartAnimation(&animatedSprites[i].animations[animatedSprites[i].currentAnimation]);
        } 
        if(keyStates[SDL_SCANCODE_3]){
            animatedSprites[i].currentAnimation = 3;
        } 
    }
}

void keyStateVelocitySetterSystem(flecs::iter &it, Velocity *velocities, KeyboardState *keyStatesCollections){
    for(auto i : it){
        u8 *keyStates = keyStatesCollections[i].keyStates;
        // side
        if(keyStates[SDL_SCANCODE_A]){
            velocities[i].x = -100;
        }
        else if(keyStates[SDL_SCANCODE_D]){
            velocities[i].x = 100;
        }
        else {
            velocities[i].x = 0;
        }

    }
}


void moveSystem(flecs::iter &it, Velocity *velocities, Position *positions){
    for(auto i : it){
        positions[i].x += velocities[i].x * it.delta_time();
        positions[i].y += velocities[i].y * it.delta_time();
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

    /**
     * SDL SETUP
     * 
     */
    SDL_Init(SDL_INIT_VIDEO);
    gWindow = SDL_CreateWindow("title",0,0, 640,480, SDL_WINDOW_SHOWN);
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Event event;

    /**
     * SETUP BACKGROUND
     * 
     */
    SDL_Surface *bgSurface = IMG_Load("bg.png");
    SDL_Texture *bgTexture = SDL_CreateTextureFromSurface(gRenderer, bgSurface);
    int bg_w = bgSurface->w;
    int bg_h = bgSurface->h;
    SDL_Rect bgDestRect = {0, 0, bg_w, bg_h}; 
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

    

    Animation runAnimation;
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
    strncpy(runAnimation.name, runAnimationName, 3);

    Animation standingAttackAnimation;
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
    
    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 0, walkAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 1, runAnimation);

    addNewAnimationToAnimatedSprite(&animatedSprite);
    overwriteAnimationOnAnimatedSprite(&animatedSprite, 2, standingAttackAnimation);

    animatedSprite.currentAnimation = 0;
    
    // ADD the setup AnimatedSprite to the pinkGuyEntity
    pinkGuyEntity.set<AnimatedSprite>(animatedSprite);
    pinkGuyEntity.set<Position>((Position){640/2,480/2});
    pinkGuyEntity.set<Velocity>((Velocity){0,0});
    
    
    // Set up the animation playing and rendering systems
    world.system<AnimatedSprite>("AnimatedSpritePlay").kind(flecs::OnUpdate).iter(animationsAccumulationSystem);

    world.system<AnimatedSprite, Position>("renderingAnimatedSprites").kind(flecs::OnStore).iter(renderingAnimatedSpritesSystem);

    world.system<AnimatedSprite, KeyboardState>("keyStateAnimationSpriteState").kind(flecs::OnUpdate).iter(keyStateAnimationSetterSystem);

    world.system<Velocity, KeyboardState>("keyStateVelocitySetter").kind(flecs::OnUpdate).iter(keyStateVelocitySetterSystem);

    world.system<Velocity, Position>("move").kind(flecs::OnUpdate).iter(moveSystem);
    // timing
    // float deltaTime = 0.0f;
    

    // main loop
    while(!quit){
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

        pinkGuyEntity.set<KeyboardState>(keyboardState);

        SDL_SetRenderDrawColor(gRenderer, 0,0,0,255);
        SDL_RenderClear(gRenderer);

        // draw background
        SDL_RenderCopy(gRenderer, bgTexture, NULL, &bgDestRect);

        world.progress();
        // animationsAccumulationSystem(&animatedSprite, 1, deltaTime);


        // renderingAnimatedSpritesSystem(&animatedSprite, &position, 1);

        //renderAnimatedSprite(640 / 2, 480 / 2, animatedSprite);

        SDL_RenderPresent(gRenderer);
    }


    
}