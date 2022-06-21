#ifndef posh
#define posh
#include "v2d.h"
#include <vector>
#include <SDL2/SDL.h>
#include "ints.h"

typedef v2d Position;


struct Angle {
    float rads;
};


// struct Position {
//     float x, y;
// };

struct SelectedForEditing {};

struct SelectedForEditingNode {
    u32 idx;
};

struct PlatformPath {
    std::vector<Position> nodes;
    bool isCircular;
};


#define degToRads(deg)(deg * (3.14/180))


#endif