#ifndef ssph
#define ssph

#include "spriteSheets.h"
#include "ints.h"
#include <stdlib.h>
#include <string>

u32 createSpriteSheet(std::string filename, size_t numCellRows, size_t numCellCols, const std::string name);
SpriteSheet getSpriteSheet(u32 id);

#endif