#include "states.h"

void State_util_set(StateCurrPrev &state, State newState){
    state.prevState = state.currentState;
    state.currentState = newState;
}


bool State_util_did_just_enter(StateCurrPrev &state, State s){
    if(s != state.currentState){
        return false;
    }
    if(s == state.prevState){
        return false;
    }
    return true;
}