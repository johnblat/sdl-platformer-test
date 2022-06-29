#ifndef velh
#define velh

struct GroundSpeed {
    float val;
};

struct Velocity {
    float x, y;
};

enum GroundMode {
    GROUND_MODE_FLOOR,
    GROUND_MODE_RIGHT_WALL,
    GROUND_MODE_CEILING,
    GROUND_MODE_LEFT_WALL
};

struct ControlLockTimer {
    float endTicks;
    float accumulator;
};

#endif