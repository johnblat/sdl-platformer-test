#include "spriteSheets.h"
#include "ints.h"
#include <stdlib.h>
#include "spriteSheet_util.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "render.h"
#include <string.h>
#include <assert.h>
#include <string>

u32 SpriteSheet_util_create(std::string filename, size_t numCellRows, size_t numCellCols, const std::string name){
    SDL_Surface *imgSurface = IMG_Load(filename.c_str());

    SDL_Texture *texture = SDL_CreateTextureFromSurface(gRenderer, imgSurface);
    SpriteSheet spriteSheet;
    spriteSheet.texture = texture;
    spriteSheet.numCellRows = numCellRows;
    spriteSheet.numCellCols = numCellCols;
    spriteSheet.w = imgSurface->w;
    spriteSheet.h = imgSurface->h;
    spriteSheet.name = name;
    u32 spriteSheetId = gNumSpriteSheets;
    gSpriteSheets[spriteSheetId] = spriteSheet; // need to make sure this has mem allocated
    gNumSpriteSheets++;

    SDL_FreeSurface(imgSurface);
    return spriteSheetId;


}

SpriteSheet SpriteSheet_util_get_by_id(u32 id){
    assert(id < gNumSpriteSheets);
    SpriteSheet ss = gSpriteSheets[id];
    assert(ss.texture != NULL);
    return ss;
}