#include "flecs.h"
#include <SDL2/SDL.h>
#include "input.h"
#include "position.h"
#include "resourceLoading.h"


void savePlatformNode(flecs::world &ecs){    
    ecs.defer_begin();

    auto f = ecs.filter<Position, PlatformPath>();

    size_t totalNumEntities = 0;
    std::vector<Position> allPositions;
    std::vector<PlatformPath> allplatformPaths;

    f.iter([&totalNumEntities, &allPositions, &allplatformPaths](flecs::iter &it,const Position *positions,const PlatformPath *platformPath){ 
        
        
        size_t numEntities = it.count();
        totalNumEntities += numEntities;

        for(i32 i : it){
            allPositions.push_back(positions[i]);
            allplatformPaths.push_back(platformPath[i]);
        }

    });

    SDL_RWops *saveContext = SDL_RWFromFile("platformVertices", "wb");
    SDL_RWwrite(saveContext, &totalNumEntities, sizeof(size_t), 1);
    SDL_RWwrite(saveContext, allPositions.data(), sizeof(Position), totalNumEntities);

    for(int i = 0; i < totalNumEntities; i++){    
        size_t vectorSize = allplatformPaths[i].nodes.size();
        SDL_RWwrite(saveContext, &vectorSize, sizeof(size_t), 1);
        const Position *vectorData = allplatformPaths[i].nodes.data();
        SDL_RWwrite(saveContext, vectorData, sizeof(Position), vectorSize);
    }

    SDL_RWclose(saveContext);
    
    ecs.defer_end();
};




void saveSystem(flecs::iter &it, Input *inputs){
    for(u64 i : it){
        if(inputIsJustReleased(inputs[i], "save")){
            flecs::world ecs = it.world();
            // auto q = ecs.query<Position, PlatformNode>();
            savePlatformNode(ecs);
            printf("SAVED!\n");
        }
    }
}


void loadInputSystem(flecs::iter &it, Input *inputs){
    for(u64 i : it){
        if(inputIsJustReleased(inputs[i], "load")){
            flecs::world ecs = it.world();
            loadPlatformPaths(ecs);
            printf("LOADED!\n");
        }
    }
}