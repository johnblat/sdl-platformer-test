#include "input.h"
#include <assert.h>
#include <iostream>
#include <string>
#include <SDL2/SDL.h>
using namespace std;

u8 *gKeyStates;

/**
 * UTILS
 */

bool Input_is_just_pressed(Input input, std::string buttonName){
    for(int i = 0; i < input.inputStates.size(); i++){
        InputMappingState buttonState = input.inputStates[i];
        if(buttonState.inputMapping.name == buttonName){
            if(buttonState.state == INPUT_IS_JUST_PRESSED){
                return true;
            }
            return false;
        }
    }
    //fprintf(stderr, "%s not found\n", buttonName.c_str());
    return false;
}


bool Input_is_pressed(Input input, std::string buttonName){
    for(int i = 0; i < input.inputStates.size(); i++){
        InputMappingState buttonState = input.inputStates.at(i);
        if(buttonState.inputMapping.name == buttonName){
            if(buttonState.state == INPUT_IS_JUST_PRESSED){
                return true;
            }
            if(buttonState.state == INPUT_IS_PRESSED){
                return true;
            }
            return false;
        }
    }
    //fprintf(stderr, "%s not found\n", buttonName.c_str());
    return false;
}


bool Input_is_just_released(Input input, std::string buttonName){
    for(int i = 0; i < input.inputStates.size(); i++){
        InputMappingState buttonState = input.inputStates[i];
        if(buttonState.inputMapping.name == buttonName){
            if(buttonState.state == INPUT_IS_JUST_RELEASED){
                return true;
            }
            return false;
        }
    }
    //fprintf(stderr, "%s not found\n", buttonName.c_str());
    return false;
}

InputMappingState InputButtonState_create_mapped_to_sdlScancode(std::string name, SDL_Scancode scanCode){
    InputMappingState ibs;
    ibs.state = INPUT_IS_NOT_PRESSED;
    ibs.state = INPUT_IS_NOT_PRESSED;
    ibs.inputMapping.type = INPUT_TYPE_KEYBOARD;
    ibs.inputMapping.name = name;
    ibs.inputMapping.sdlScancode = scanCode;

    return ibs;
}

void Input_append_new_input_button_state_mapped_to_sdlScancode(Input &input, std::string buttonName, SDL_Scancode scancode){
    input.inputStates.push_back(InputButtonState_create_mapped_to_sdlScancode(buttonName, scancode));
}

/**
 * SYSTEMS
 */

void Input_update_input_button_states_System(flecs::iter &it, Input *inputs){
    for(auto i : it){

        for(int j = 0; j < inputs[i].inputStates.size(); j++){

            InputMappingState bs = inputs[i].inputStates[j];

            if(gKeyStates[bs.inputMapping.sdlScancode]){
                if(bs.state == INPUT_IS_JUST_PRESSED){
                    bs.state = INPUT_IS_PRESSED;
                }
                else if(bs.state == INPUT_IS_NOT_PRESSED){
                    bs.state = INPUT_IS_JUST_PRESSED;
                }
                else if(bs.state == INPUT_IS_JUST_RELEASED){
                    bs.state = INPUT_IS_JUST_PRESSED;
                }
                else if(bs.state == INPUT_IS_PRESSED){
                    bs.state = INPUT_IS_PRESSED;
                }
            } 
            else {
                if(bs.state == INPUT_IS_JUST_PRESSED){
                    bs.state = INPUT_IS_JUST_RELEASED;
                }
                else if(bs.state == INPUT_IS_NOT_PRESSED){
                    bs.state = INPUT_IS_NOT_PRESSED;
                }
                else if(bs.state == INPUT_IS_JUST_RELEASED){
                    bs.state = INPUT_IS_NOT_PRESSED;
                }
                else if(bs.state == INPUT_IS_PRESSED){
                    bs.state = INPUT_IS_JUST_RELEASED;
                }
            }
            inputs[i].inputStates[j] = bs;
        }
    }
}