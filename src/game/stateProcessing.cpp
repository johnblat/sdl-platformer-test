#include "states.h"

void setState(StateCurrPrev &state, State newState){
    state.prevState = state.currentState;
    state.currentState = newState;
}


bool stateJustEntered(StateCurrPrev &state, State s){
    if(s != state.currentState){
        return false;
    }
    if(s == state.prevState){
        return false;
    }
    return true;
}