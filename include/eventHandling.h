#pragma once

#include <SDL2/SDL.h>

void registerEventProcessor(void (*function)(SDL_Event &event));
void runRegisteredEventProcessors(SDL_Event &event);