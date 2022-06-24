#pragma once

#include "spriteSheets.h"
#include "ints.h"
#include <stdlib.h>
#include <string>

u32 SpriteSheet_util_create(std::string filename, size_t numCellRows, size_t numCellCols, const std::string name);
SpriteSheet SpriteSheet_util_get_by_id(u32 id);

