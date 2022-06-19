#include "flecs.h"
#include <SDL2/SDL.h>
#include "input.h"
#include "position.h"
#include "resourceLoading.h"


void savePlatformNode(flecs::world &ecs){    
    ecs.defer_begin();

    auto f = ecs.filter<Position, PlatformNodeCollection>();

    size_t totalNumEntities = 0;
    std::vector<Position> allPositions;
    std::vector<PlatformNodeCollection> allpncs;

    f.iter([&totalNumEntities, &allPositions, &allpncs](flecs::iter &it,const Position *positions,const PlatformNodeCollection *pnc){ 
        
        
        size_t numEntities = it.count();
        totalNumEntities += numEntities;

        for(i32 i : it){
            allPositions.push_back(positions[i]);
            allpncs.push_back(pnc[i]);
        }

    });

    SDL_RWops *saveContext = SDL_RWFromFile("platformVertices", "wb");
    SDL_RWwrite(saveContext, &totalNumEntities, sizeof(size_t), 1);
    SDL_RWwrite(saveContext, allPositions.data(), sizeof(Position), totalNumEntities);

    for(int i = 0; i < totalNumEntities; i++){    
        size_t vectorSize = allpncs[i].vals.size();
        SDL_RWwrite(saveContext, &vectorSize, sizeof(size_t), 1);
        const Position *vectorData = allpncs[i].vals.data();
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
            loadPlatformNode(ecs);
            printf("LOADED!\n");
        }
    }
}