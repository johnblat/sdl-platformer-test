#include "states.h"

/**
 * @brief Set the State object so that prev and curr states are recorded
 * 
 * @param state 
 * @param newState 
 */
void State_util_set(StateCurrPrev &state, State newState);


/**
 * @brief Was s just entered in the state. Basically means that the previous state is different
 * 
 * @param state 
 * @param s 
 * @return true 
 * @return false 
 */
bool State_util_did_just_enter(StateCurrPrev &state, State s);