#include <flecs.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <position.h>
#include "ints.h"

template<typename T> 
void copyDynamicArrayToVector(T *arr, size_t size, std::vector<T> &vec){
    for(int i = 0; i < size; i++){
        vec.push_back(arr[i]);
    }
}

struct PVsDynamicArray {
    size_t count;
    PlatformVertex *pvs;
};


struct PlatformVerticesFileDefintion {
    size_t numEntities;
    Position *positions;
    PVsDynamicArray *PVsDynamicArrays;
};


void loadPlatformVertices(flecs::world &ecs){

    //ecs.defer_begin();

    SDL_RWops *loadContext = SDL_RWFromFile("platformVertices", "rb");
    i32 numEntities = 0;

    SDL_RWread(loadContext, &numEntities, sizeof(i32), 1);
    Position *positions = (Position *)malloc(sizeof(Position)*numEntities);
    PlatformVertices *platformVerticesCollection = (PlatformVertices *)calloc(numEntities, sizeof(PlatformVertices)); 
    SDL_RWread(loadContext, positions, sizeof(Position), numEntities);



    for(int i = 0; i < numEntities; i++){
        size_t count = 0;
        SDL_RWread(loadContext, &count, sizeof(size_t), 1);

        PlatformVertex *platformVertices = (PlatformVertex *)malloc(sizeof(PlatformVertex) * count);
        SDL_RWread(loadContext, platformVertices, sizeof(PlatformVertex), count);

        PlatformVertices pvs;
        pvs.color = (SDL_Color){255,255,255,255};
    
        copyDynamicArrayToVector<PlatformVertex>(platformVertices, count, pvs.vals);


        platformVerticesCollection[i].color = pvs.color;
        platformVerticesCollection[i].vals = pvs.vals;


        free(platformVertices);
    }

    SDL_RWclose(loadContext);
    

    for(int i = 0; i < numEntities; i++){
        flecs::entity e = ecs.entity();
        e.set<Position>(positions[i]);
        e.set<PlatformVertices>(platformVerticesCollection[i]);
    }

    free(platformVerticesCollection);
    free(positions);

   //ecs.defer_end();

}