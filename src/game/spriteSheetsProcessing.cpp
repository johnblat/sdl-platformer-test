#include "spriteSheets.h"
#include "ints.h"
#include <stdlib.h>
#include "spriteSheetsProcessing.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "render.h"
#include <string.h>
#include <assert.h>
#include <string>

u32 createSpriteSheet(const char *filename, size_t numCellRows, size_t numCellCols, const std::string name){
    SDL_Surface *imgSurface = IMG_Load(filename);

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

SpriteSheet getSpriteSheet(u32 id){
    assert(id < gNumSpriteSheets);
    SpriteSheet ss = gSpriteSheets[id];
    assert(ss.texture != NULL);
    return ss;
}