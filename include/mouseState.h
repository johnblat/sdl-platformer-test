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
    InputState lmbCurrentState;
    InputState lmbPreviousState;
    InputState rmbCurrentState;
    InputState rmbPreviousState;
    MouseWheelState currentMouseWheelState;
    MouseWheelState previousMouseWheelState;

};
