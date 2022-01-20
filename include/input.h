#ifndef inputh
#define inputh

#include "jbInts.h"
#include <SDL2/SDL.h>
#include <string>
#include "flecs.h"

/**
 * @brief Contains the current key states of the game
 * Ought to be updated every frame with SDL_PollEvent
 * 
 */
extern u8 *gKeyStates;



struct KeyboardState{
    u8 *keyStates;
};


typedef enum InputState {
    INPUT_IS_JUST_PRESSED,
    INPUT_IS_PRESSED,
    INPUT_IS_JUST_RELEASED,
    INPUT_IS_NOT_PRESSED
};


typedef struct InputButtonState {
    std::string name;
    SDL_Scancode sdlScancode;
    InputState currentInputState;
    InputState previousInputState;
};


typedef struct Input Input;
struct Input {
    InputButtonState *buttonStates;
    u64 numButtomStates;
};


/**
 * UTILS
 */
bool isJustPressed(Input input, std::string buttonName);
bool isPressed(Input input, std::string buttonName);
bool isJustReleased(Input input, std::string buttonName);


/**
 * SYSTEMS
 */
void inputUpdateSystem(flecs::iter &it, Input *inputs);


#endif