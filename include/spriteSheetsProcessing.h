#ifndef ssph
#define ssph

#include "spriteSheets.h"
#include "ints.h"
#include <stdlib.h>
#include <string>

u32 createSpriteSheet(const char *filename, size_t numCellRows, size_t numCellCols, std::string name);
SpriteSheet getSpriteSheet(u32 id);

#endif