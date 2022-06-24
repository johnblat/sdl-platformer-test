#include "timestep.h"
#include <SDL2/SDL.h>
#include "ints.h"

TimeStep gTimeStep;


TimeStep ts_TimeStep_init(float FPS){
    TimeStep ts;
    ts.secondsPerFrame = 1.0f / FPS;
    ts.startTicks = 0;
    return ts;
}


void ts_TimeStep_start_ticks_set_to_current_ticks(TimeStep &ts){
    ts.startTicks = SDL_GetTicks();
}


void ts_TimeStep_delay_remaining_time(TimeStep &ts){
    float endTicks = SDL_GetTicks();
    float totalTicks = endTicks - ts.startTicks;
    float totalSeconds = totalTicks / 1000.0f;

    if(totalSeconds < ts.secondsPerFrame){
        float secondsRemainingToFixTimeStep = ts.secondsPerFrame - totalSeconds;
        float msRemainingToFixTimeStep = secondsRemainingToFixTimeStep * 1000;
        SDL_Delay((u32)msRemainingToFixTimeStep);
    }
}