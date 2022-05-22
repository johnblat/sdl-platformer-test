#ifndef velh
#define velh

struct GroundSpeed {
    float val;
};

struct Velocity {
    float x, y;
};

enum GroundMode {
    FLOOR_GM,
    RIGHT_WALL_GM,
    CEILING_GM,
    LEFT_WALL_GM
};

#endif