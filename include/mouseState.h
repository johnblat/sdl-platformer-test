#pragma once

#include "input.h"
#include "position.h"


enum MouseWheelState {
    SCROLL_UP,
    SCROLL_DOWN,
    NOT_SCROLLING
};


struct MouseState {
    Position windowPosition;
    Position logicalPosition;
    Position worldPosition;
    InputStateType lmbCurrentState;
    InputStateType lmbPreviousState;
    InputStateType rmbCurrentState;
    InputStateType rmbPreviousState;
    MouseWheelState currentMouseWheelState;
    MouseWheelState previousMouseWheelState;

};
