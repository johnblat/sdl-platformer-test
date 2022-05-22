#include "flecs.h"
#include <SDL2/SDL.h>
#include "input.h"
#include "position.h"
#include "resourceLoading.h"


void savePlatformVertices(flecs::world &ecs){    
    ecs.defer_begin();

    auto f = ecs.filter<Position, PlatformVertices>();

    size_t totalNumEntities = 0;
    std::vector<Position> allPositions;
    std::vector<PlatformVertices> allPvss;

    f.iter([&totalNumEntities, &allPositions, &allPvss](flecs::iter &it,const Position *positions,const PlatformVertices *pvs){ 
        
        
        size_t numEntities = it.count();
        totalNumEntities += numEntities;

        for(i32 i : it){
            allPositions.push_back(positions[i]);
            allPvss.push_back(pvs[i]);
        }

    });

    SDL_RWops *saveContext = SDL_RWFromFile("platformVertices", "wb");
    SDL_RWwrite(saveContext, &totalNumEntities, sizeof(size_t), 1);
    SDL_RWwrite(saveContext, allPositions.data(), sizeof(Position), totalNumEntities);

    for(int i = 0; i < totalNumEntities; i++){    
        size_t vectorSize = allPvss[i].vals.size();
        SDL_RWwrite(saveContext, &vectorSize, sizeof(size_t), 1);
        const Position *vectorData = allPvss[i].vals.data();
        SDL_RWwrite(saveContext, vectorData, sizeof(Position), vectorSize);
    }

    SDL_RWclose(saveContext);
    
    ecs.defer_end();
};




void saveSystem(flecs::iter &it, Input *inputs){
    for(u64 i : it){
        if(inputIsJustReleased(inputs[i], "save")){
            flecs::world ecs = it.world();
            // auto q = ecs.query<Position, PlatformVertices>();
            savePlatformVertices(ecs);
            printf("SAVED!\n");
        }
    }
}


void loadInputSystem(flecs::iter &it, Input *inputs){
    for(u64 i : it){
        if(inputIsJustReleased(inputs[i], "load")){
            flecs::world ecs = it.world();
            loadPlatformVertices(ecs);
            printf("LOADED!\n");
        }
    }
}