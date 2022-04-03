#ifndef states_h
#define states_h


enum State {
    STATE_IN_AIR,
    STATE_ON_GROUND
};

struct StateCurrPrev {
    State currentState;
    State prevState;
};

#endif