#include <vector>
#include "eventHandling.h"


static std::vector<void (*)(SDL_Event &event)> gEventProcessors;


void registerEventProcessor(void (*function)(SDL_Event &event)){
    gEventProcessors.push_back(function);
}


void runRegisteredEventProcessors(SDL_Event &event){
    for(auto ep : gEventProcessors){
        ep(event);
    }
}