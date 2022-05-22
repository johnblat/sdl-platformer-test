#ifndef inputh
#define inputh

#include "ints.h"
#include <SDL2/SDL.h>
#include <string>
#include "flecs.h"
#include <vector>

/**
 * @brief Contains the current key states of the game
 * Ought to be updated every frame with SDL_PollEvent
 * 
 */
extern u8 *gKeyStates;



struct KeyboardState{
    u8 *keyStates;
};

enum InputState {
    INPUT_IS_JUST_PRESSED,
    INPUT_IS_PRESSED,
    INPUT_IS_JUST_RELEASED,
    INPUT_IS_NOT_PRESSED
};

typedef struct InputButtonState InputButtonState;
struct InputButtonState {
    std::string name;
    SDL_Scancode sdlScancode;
    InputState currentInputState;
    InputState previousInputState;
};


typedef struct Input Input;
struct Input {
    std::vector<InputButtonState> buttonStates;
};


/**
 * UTILS
 */
bool inputIsJustPressed(Input input, std::string buttonName);
bool inputIsPressed(Input input, std::string buttonName);
bool inputIsJustReleased(Input input, std::string buttonName);
InputButtonState createbuttonState(std::string name, SDL_Scancode scanCode);
void addButtonToInput(Input &input, std::string buttonName, SDL_Scancode scancode);

/**
 * SYSTEMS
 */
void inputUpdateSystem(flecs::iter &it, Input *inputs);


#endif