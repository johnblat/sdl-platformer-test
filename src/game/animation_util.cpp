#include "animation.h"
#include "render.h"
#include "window.h"
#include "spriteSheet_util.h"
#include "animation_util.h"
#include "position.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "flecs.h"
#include "input.h"
#include "velocity.h"
#include "camera.h"
#include "state_util.h"
#include "util.h"



void anim_util_AnimatedSprite_play(AnimatedSprite *as, std::string animationName){
    for(int i = 0; i < as->numAnimations; i++){
        if(as->animations[i].name == animationName){
            as->currentAnimation = i;
            return;
        }
    }
    printf("animation name: %s\n", animationName.c_str());
    assert(0 && "Animation name was not found in list of animations");
}

AnimatedSprite anim_util_AnimatedSprite_create(u32 spriteSheetId){
    AnimatedSprite animatedSprite;
    animatedSprite.numAnimations = 0;
    animatedSprite.currentAnimation = 0;
    animatedSprite.spriteSheetId = spriteSheetId;
    return animatedSprite;
}


void anim_util_AnimatedSprite_add_blank_Animation(AnimatedSprite *animatedSprite){
    Animation animation;
    animation.currentFrame = 0;
    animation.accumulator = 0.0f;
    animation.fps = 0.0f;
    animation.msPerFrame = 0.0f;

    animatedSprite->animations[animatedSprite->numAnimations] = animation;
    animatedSprite->numAnimations++;
}

void anim_util_AnimatedSprite_add_Animation(AnimatedSprite &animatedSprite, Animation animation){
    animatedSprite.animations[animatedSprite.numAnimations] = animation;
    animatedSprite.numAnimations++;
}

void anim_util_AnimatedSprite_overwrite_Animation_at_index(AnimatedSprite *animatedSprite, u32 animationIndex, Animation animation){
    animatedSprite->animations[animationIndex] = animation;
}


void anim_util_Animation_restart(Animation *animation){
    animation->currentFrame = 0;
}


bool anim_util_Animation_has_reached_final_frame(Animation *animation){
    return animation->currentFrame >= animation->frameIndeces.size();
}



Animation anim_util_Animation_init(std::vector<u32> frameIndices, float FPS, bool isLoop, std::string name){
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