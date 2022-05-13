#ifndef animh
#define animh

#include "ints.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string>
#include <vector>


struct Animation {
    std::string name; // animation name
    std::vector<u32> frameIndeces;
    u32 currentFrame; // current index into arrFrames
    float accumulator; // current time through the current frame
    float fps; // frames per second
    float msPerFrame; // time spent per frame. Accumulator can use this
    bool isLoop; 
};

struct AnimatedSprite { 
    char filename[64]; 
    std::string spriteSheetName;
    u32 spriteSheetId; // Id into the global array of sprite sheets
    u32 numAnimations;
    Animation animations[8];// collection of animations for this animated sprite
    u32 currentAnimation;
    SDL_RendererFlip flip;
};

#endif