#include "timestep.h"
#include <SDL2/SDL.h>
#include "ints.h"


TimeStep TimeStepInit(float FPS){
    TimeStep ts;
    ts.secondsPerFrame = 1.0f / FPS;
    ts.startTicks = 0;
    return ts;
}


void TimeStepSetStartTicks(TimeStep &ts){
    ts.startTicks = SDL_GetTicks();
}

void TimeStepSkip(TimeStep &ts){
    float endTicks = SDL_GetTicks();
    float totalTicks = endTicks - ts.startTicks;
    float totalSeconds = totalTicks / 1000.0f;

    if(totalSeconds < ts.secondsPerFrame){
        float secondsRemainingToFixTimeStep = ts.secondsPerFrame - totalSeconds;
        float msRemainingToFixTimeStep = secondsRemainingToFixTimeStep * 1000;
        SDL_Delay((u32)msRemainingToFixTimeStep);
    }
}