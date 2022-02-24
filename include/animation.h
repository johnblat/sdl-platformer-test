#ifndef animh
#define animh

#include "ints.h"
#include <stdlib.h>
#include <SDL2/SDL.h>

struct Animation {
    char name[32]; // animation name
    size_t numFrames;
    u32 arrFrames[16]; // indexes into some sprite sheet. should be made dynamic eventually
    u32 currentFrame; // current index into arrFrames
    float accumulator; // current time through the current frame
    float fps; // frames per second
    float msPerFrame; // time spent per frame. Accumulator can use this
    bool isLoop; 
};

struct AnimatedSprite { 
    char filename[64]; 
    char spriteSheetName[64];
    u32 spriteSheetId; // Id into the global array of sprite sheets
    u32 numAnimations;
    Animation animations[8];// collection of animations for this animated sprite
    u32 currentAnimation;
    SDL_RendererFlip flip;
};

#endif