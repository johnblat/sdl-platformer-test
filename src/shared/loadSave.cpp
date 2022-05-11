#include "flecs.h"
#include <SDL2/SDL.h>
#include "input.h"
#include "position.h"
#include "resourceLoading.h"


void savePlatformVertices(flecs::world &ecs){    
    ecs.defer_begin();

    auto f = ecs.filter<Position, PlatformVertices>();

    f.iter([](flecs::iter &it,const Position *positions,const PlatformVertices *pvs){ 
        SDL_RWops *saveContext = SDL_RWFromFile("platformVertices", "wb");
        
        i32 numEntities = it.count();
        SDL_RWwrite(saveContext, &numEntities, sizeof(i32), 1);
        SDL_RWwrite(saveContext, positions, sizeof(Position), it.count());

        for(int i = 0; i < numEntities; i++){    
            size_t vectorSize = pvs[i].vals.size();
            SDL_RWwrite(saveContext, &vectorSize, sizeof(size_t), 1);
            const Position *vectorData = pvs[i].vals.data();
            SDL_RWwrite(saveContext, vectorData, sizeof(Position), vectorSize);
        }

        SDL_RWclose(saveContext);
    });

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