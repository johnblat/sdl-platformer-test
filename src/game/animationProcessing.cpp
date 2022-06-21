#include "animation.h"
#include "render.h"
#include "window.h"
#include "spriteSheetsProcessing.h"
#include "animationProcessing.h"
#include "position.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "flecs.h"
#include "input.h"
#include "velocity.h"
#include "camera.h"
#include "stateProcessing.h"
#include "util.h"
//
// HELPER
//
u32 RCToI(u32 row, u32 col, u32 numRows){
    return (row * numRows) + col;
}


void iToRC(u32 i, u32 numRows, u32 *row, u32 *col){
    *row = i / (numRows);
    *col = i % (numRows);
}


//
// SYSTEMS
//
void animationsAccumulationSystem(flecs::iter &it, AnimatedSprite *animatedSprites){

    for(auto i : it){
        u32 currentAnimation = animatedSprites[i].currentAnimation;

        if(isAnimationFinished(&animatedSprites[i].animations[currentAnimation])){
            if(animatedSprites[i].animations[currentAnimation].isLoop == false) continue;
            animatedSprites[i].animations[currentAnimation].currentFrame = 0;
        }

        animatedSprites[i].animations[currentAnimation].accumulator += it.delta_time();

        if(animatedSprites[i].animations[currentAnimation].accumulator > animatedSprites[i].animations[currentAnimation].msPerFrame){            
            animatedSprites[i].animations[currentAnimation].accumulator = 0.0f;
            animatedSprites[i].animations[currentAnimation].currentFrame++;

            
        }
    }
    
    
}



void renderingAnimatedSpritesSystem(flecs::iter &it, AnimatedSprite *animatedSprites, Position *positions, Angle *angles){
    for(auto i : it){
        SpriteSheet spriteSheet = getSpriteSheet(animatedSprites[i].spriteSheetId);
        float cell_h = spriteSheet.h / spriteSheet.numCellRows;
        float cell_w = spriteSheet.w / spriteSheet.numCellCols;

        float scale;
        SDL_RenderGetScale(gRenderer,&scale, NULL );
        gCenterScreen = {(float)gScreenWidth/2.0f, (float)gScreenHeight/2.0f};
        Position scaledCenterScreen = {gCenterScreen.x / scale, gCenterScreen.y / scale};
        
        SDL_Rect renderRect;
        renderRect.x = (positions[i].x - gCameraPosition.x + scaledCenterScreen.x) - (cell_w*0.5f);
        renderRect.y = (positions[i].y - gCameraPosition.y + scaledCenterScreen.y) - (cell_h*0.5f);
        
        renderRect.w = cell_w;
        renderRect.h = cell_h;
        

        Animation currentAnimation = animatedSprites[i].animations[animatedSprites[i].currentAnimation];
        SDL_Rect srcRect;
        u32 spriteSheetCellIndex = currentAnimation.frameIndeces[currentAnimation.currentFrame];
        u32 cell_r;
        u32 cell_c;
        iToRC(spriteSheetCellIndex, spriteSheet.numCellCols, &cell_r, &cell_c);
        srcRect.x = cell_c * cell_w;
        srcRect.y = cell_r * cell_h;
        srcRect.w = cell_w;
        srcRect.h = cell_h;
        SDL_RendererFlip flip = animatedSprites[i].flip == SDL_FLIP_HORIZONTAL ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE; 
        SDL_RenderCopyEx(gRenderer, spriteSheet.texture, &srcRect, &renderRect, -rads2deg(angles[i].rads), NULL, flip);
    }
    
}


void KeyboardStateAnimationSetterSystem(flecs::iter &it, AnimatedSprite *animatedSprites, KeyboardState *keyStatesCollections){
    for(auto i : it){
        u8 *keyStates = keyStatesCollections[i].keyStates;
        if(keyStates[SDL_SCANCODE_0]){
            animatedSpritePlay(&animatedSprites[i], "walk");
        } 
        if(keyStates[SDL_SCANCODE_1]){
            animatedSpritePlay(&animatedSprites[i], "run");

        } 
        if(keyStates[SDL_SCANCODE_2]){
            animatedSpritePlay(&animatedSprites[i], "stand-attack");
            restartAnimation(&animatedSprites[i].animations[animatedSprites[i].currentAnimation]);
        }
        if(keyStates[SDL_SCANCODE_3]){
            animatedSpritePlay(&animatedSprites[i], "idle");
        }
        // if(keyStates[SDL_SCANCODE_3]){
        //     animatedSprites[i].currentAnimation = 3;
        // } 
    }
}


void InputFlipSystem(flecs::iter &it, AnimatedSprite *animatedSprites, Input *inputs){
    for(auto i : it){

        if(Input_is_pressed(inputs[i], "left")){
            animatedSprites[i].flip = SDL_FLIP_HORIZONTAL;
        }
        else if(Input_is_pressed(inputs[i], "right")){
            animatedSprites[i].flip = SDL_FLIP_NONE;
        }
    }
}

void setAnimationBasedOnSpeedSystem(flecs::iter &it, AnimatedSprite *animatedSprites, Velocity *velocities, StateCurrPrev *states){
    for(int i : it){
        // set animation
        
        if(states[i].currentState == STATE_IN_AIR){
            animatedSpritePlay(&animatedSprites[i], "jump");
            if(stateJustEntered(states[i], STATE_IN_AIR)){
                restartAnimation(&animatedSprites[i].animations[animatedSprites[i].currentAnimation]);
            }
        }
        else if(velocities[i].x == 0){
            animatedSpritePlay(&animatedSprites[i], "idle");
        }
        else if(velocities[i].x < 5.0f && velocities[i].x > -5.0f){
            animatedSpritePlay(&animatedSprites[i], "walk");
        }
        else {
            animatedSpritePlay(&animatedSprites[i], "run");
        }
    }
}


// 
// MANAGING, LOADING, CREATION, ETC
// 

void animatedSpritePlay(AnimatedSprite *as, std::string animationName){
    for(int i = 0; i < as->numAnimations; i++){
        if(as->animations[i].name == animationName){
            as->currentAnimation = i;
            return;
        }
    }
    printf("animation name: %s\n", animationName.c_str());
    assert(0 && "Animation name was not found in list of animations");
}

AnimatedSprite createAnimatedSprite(u32 spriteSheetId){
    AnimatedSprite animatedSprite;
    animatedSprite.numAnimations = 0;
    animatedSprite.currentAnimation = 0;
    animatedSprite.spriteSheetId = spriteSheetId;
    return animatedSprite;
}


void addNewAnimationToAnimatedSprite(AnimatedSprite *animatedSprite){
    Animation animation;
    animation.currentFrame = 0;
    animation.accumulator = 0.0f;
    animation.fps = 0.0f;
    animation.msPerFrame = 0.0f;

    animatedSprite->animations[animatedSprite->numAnimations] = animation;
    animatedSprite->numAnimations++;
}

void addAnimationToAnimatedSprite(AnimatedSprite &animatedSprite, Animation animation){
    animatedSprite.animations[animatedSprite.numAnimations] = animation;
    animatedSprite.numAnimations++;
}

void overwriteAnimationOnAnimatedSprite(AnimatedSprite *animatedSprite, u32 animationIndex, Animation animation){
    animatedSprite->animations[animationIndex] = animation;
}


void restartAnimation(Animation *animation){
    animation->currentFrame = 0;
}


bool isAnimationFinished(Animation *animation){
    return animation->currentFrame >= animation->frameIndeces.size() - 1;
}



Animation createAnimation(std::vector<u32> frameIndices, float FPS, bool isLoop, std::string name){
    Animation a;
    a.frameIndeces = frameIndices;
    a.fps = FPS;
    a.msPerFrame = 1.0 / FPS;
    a.isLoop = isLoop;
    a.name = name;
    a.currentFrame = 0;
    a.accumulator = 0;
    return a;
}