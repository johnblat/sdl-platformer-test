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

enum InputStateType {
    INPUT_IS_JUST_PRESSED,
    INPUT_IS_PRESSED,
    INPUT_IS_JUST_RELEASED,
    INPUT_IS_NOT_PRESSED
};


enum InputType {
    INPUT_TYPE_KEYBOARD,
    INPUT_TYPE_GAME_CONTROLLER,
    INPUT_TYPE_MOUSE
};



struct GameControllerButtonReference {
    SDL_GameControllerButton sdlGameControllerButton;
    SDL_GameController *gameController;
};

struct InputMapping {
    InputType type;
    std::string name;
    union {
        SDL_Scancode sdlScancode;
        GameControllerButtonReference gameControllerButtonRef;
    };
};

struct InputMappingState {
    InputMapping inputMapping;
    InputStateType state;
};


struct Input {
    std::vector<InputMappingState> inputStates;
};


/**
 * UTILS
 */
bool Input_is_just_pressed(Input input, std::string buttonName);
bool Input_is_pressed(Input input, std::string buttonName);
bool Input_is_just_released(Input input, std::string buttonName);
void Input_append_new_input_button_state_mapped_to_sdlScancode(Input &input, std::string buttonName, SDL_Scancode scancode);
InputMappingState InputButtonState_create_mapped_to_sdlScancode(std::string name, SDL_Scancode scanCode);

/**
 * SYSTEMS
 */
void Input_update_input_button_states_System(flecs::iter &it, Input *inputs);


#endif