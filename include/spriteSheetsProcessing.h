#ifndef ssph
#define ssph

#include "spriteSheets.h"
#include "jbInts.h"
#include <stdlib.h>

u32 createSpriteSheet(const char *filename, size_t numCellRows, size_t numCellCols, const char *name);
SpriteSheet getSpriteSheet(u32 id);

#endif