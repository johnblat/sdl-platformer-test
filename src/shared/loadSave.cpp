#include "flecs.h"
#include <SDL2/SDL.h>
#include "input.h"
#include "position.h"
#include "util.h"

void save_PlatformPaths(flecs::world &ecs){    
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


void 
load_PlatformPaths(flecs::world &ecs){
    SDL_RWops *loadContext = SDL_RWFromFile("platformVertices", "rb");
    if(loadContext == NULL){
        printf("No file!\n");
        return;
    }

    size_t numEntities = 0;

    SDL_RWread(loadContext, &numEntities, sizeof(size_t), 1);
    Position *positions = (Position *)malloc(sizeof(Position)*numEntities);
    PlatformPath *platformPaths = (PlatformPath *)calloc(numEntities, sizeof(PlatformPath)); 
    SDL_RWread(loadContext, positions, sizeof(Position), numEntities);



    for(int i = 0; i < numEntities; i++){
        size_t count = 0;
        SDL_RWread(loadContext, &count, sizeof(size_t), 1);

        Position *platformVertices = (Position *)malloc(sizeof(Position) * count);
        SDL_RWread(loadContext, platformVertices, sizeof(Position), count);

        PlatformPath platformPath;
    
        util_dynamic_array_to_vector<Position>(platformVertices, count, platformPath.nodes);

        platformPaths[i].nodes = platformPath.nodes;


        free(platformVertices);
    }

    SDL_RWclose(loadContext);
    

    for(int i = 0; i < numEntities; i++){
        flecs::entity e = ecs.entity();
        e.set<Position>(positions[i]);
        e.set<PlatformPath>(platformPaths[i]);
    }

    free(platformPaths);
    free(positions);
}



void save_PlatformPaths_on_save_button_release_System(flecs::iter &it, Input *inputs){
    for(u64 i : it){
        if(Input_is_just_released(inputs[i], "save")){
            flecs::world ecs = it.world();
            // auto q = ecs.query<Position, PlatformNode>();
            save_PlatformPaths(ecs);
            printf("SAVED!\n");
        }
    }
}


void load_PlatformPaths_on_load_button_release_System(flecs::iter &it, Input *inputs){
    for(u64 i : it){
        if(Input_is_just_released(inputs[i], "load")){
            flecs::world ecs = it.world();
            load_PlatformPaths(ecs);
            printf("LOADED!\n");
        }
    }
}