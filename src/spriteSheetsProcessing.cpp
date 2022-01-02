#include "spriteSheets.h"
#include "jbInts.h"
#include <stdlib.h>
#include "spriteSheetsProcessing.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "render.h"
#include <string.h>
#include <assert.h>


u32 createSpriteSheet(const char *filename, size_t numCellRows, size_t numCellCols, const char *name){
    SDL_Surface *imgSurface = IMG_Load(filename);
    Uint32 alphaColor = 0x234c73;

    // for(int y = 0; y < imgSurface->h; y++){
    //     for(int x = 0; x < imgSurface->w; x++){
    //         SDL_PixelFormat format = *(imgSurface->format);
    //         Uint8 *pixels = (Uint8 *)imgSurface->pixels;
    //         Uint8 pixel = pixels[y*imgSurface->w + x];
    //         if(pixel == alphaColor){
    //             pixels[y*imgSurface->h + x] = 0x0;
    //         }
    //     }
    // }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(gRenderer, imgSurface);
    SpriteSheet spriteSheet;
    spriteSheet.texture = texture;
    spriteSheet.numCellRows = numCellRows;
    spriteSheet.numCellCols = numCellCols;
    spriteSheet.w = imgSurface->w;
    spriteSheet.h = imgSurface->h;
    strncpy(spriteSheet.name, name, 64);
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