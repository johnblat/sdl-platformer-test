#ifndef sprsheeth
#define sprsheeth

#include <SDL2/SDL.h>
#include "ints.h"

struct SpriteSheet {
    char name[64];
    SDL_Texture *texture;
    size_t w;
    size_t h;
    size_t numCellRows;
    size_t numCellCols;
};   


/**
 * The main idea here is that these variables will store the sprite sheets that are active in the game.
 * Other Objects that reference a sprite sheet will do so by name or id.
 * 
 * The reason is that many objects might share a sprite sheet. For example enemies,
 * or perhaps any number of game objects or props might reference the same sprite sheet.
 * 
 * We could store a pointer in all of these objects, however, if that pointer goes haywire we handle that
 * in a better way by accessing the spriteSheets by idx or name in an accessor function.
 * 
 * This way something can be returned such as a placeholder texture, or we can make the system just outright fail if no texture exists at that index.
 * 
 * Instead of handling NULL textures all over the place, we can handle it in once place.
 * 
 * IDK... That's the idea anyway.
 * 
 */
#define MAX_SPRITE_SHEETS 16
extern SpriteSheet gSpriteSheets[MAX_SPRITE_SHEETS]; // eventually make this dynamic
extern size_t gNumSpriteSheets;
#endif