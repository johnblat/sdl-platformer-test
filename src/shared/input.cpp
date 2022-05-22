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

bool inputIsJustPressed(Input input, std::string buttonName){
    for(int i = 0; i < input.buttonStates.size(); i++){
        InputButtonState buttonState = input.buttonStates[i];
        if(buttonState.name == buttonName){
            if(buttonState.currentInputState == INPUT_IS_JUST_PRESSED){
                return true;
            }
            return false;
        }
    }
    fprintf(stderr, "%s not found\n", buttonName.c_str());
    return false;
}


bool inputIsPressed(Input input, std::string buttonName){
    for(int i = 0; i < input.buttonStates.size(); i++){
        InputButtonState buttonState = input.buttonStates.at(i);
        if(buttonState.name == buttonName){
            if(buttonState.currentInputState == INPUT_IS_JUST_PRESSED){
                return true;
            }
            if(buttonState.currentInputState == INPUT_IS_PRESSED){
                return true;
            }
            return false;
        }
    }
    fprintf(stderr, "%s not found\n", buttonName.c_str());
    return false;
}


bool inputIsJustReleased(Input input, std::string buttonName){
    for(int i = 0; i < input.buttonStates.size(); i++){
        InputButtonState buttonState = input.buttonStates[i];
        if(buttonState.name == buttonName){
            if(buttonState.currentInputState == INPUT_IS_JUST_RELEASED){
                return true;
            }
            return false;
        }
    }
    fprintf(stderr, "%s not found\n", buttonName.c_str());
    return false;
}

InputButtonState createbuttonState(std::string name, SDL_Scancode scanCode){
    InputButtonState ibs;
    ibs.currentInputState = INPUT_IS_NOT_PRESSED;
    ibs.previousInputState = INPUT_IS_NOT_PRESSED;
    ibs.name = name;
    ibs.sdlScancode = scanCode;

    return ibs;
}

void addButtonToInput(Input &input, std::string buttonName, SDL_Scancode scancode){
    input.buttonStates.push_back(createbuttonState(buttonName, scancode));
}

/**
 * SYSTEMS
 */

void inputUpdateSystem(flecs::iter &it, Input *inputs){
    for(auto i : it){

        for(int j = 0; j < inputs[i].buttonStates.size(); j++){

            InputButtonState bs = inputs[i].buttonStates[j];
            bs.previousInputState = bs.currentInputState;

            if(gKeyStates[bs.sdlScancode]){
                if(bs.previousInputState == INPUT_IS_JUST_PRESSED){
                    bs.currentInputState = INPUT_IS_PRESSED;
                }
                else if(bs.previousInputState == INPUT_IS_NOT_PRESSED){
                    bs.currentInputState = INPUT_IS_JUST_PRESSED;
                }
                else if(bs.previousInputState == INPUT_IS_JUST_RELEASED){
                    bs.currentInputState = INPUT_IS_JUST_PRESSED;
                }
                else if(bs.previousInputState == INPUT_IS_PRESSED){
                    bs.currentInputState = INPUT_IS_PRESSED;
                }
            } 
            else {
                if(bs.previousInputState == INPUT_IS_JUST_PRESSED){
                    bs.currentInputState = INPUT_IS_JUST_RELEASED;
                }
                else if(bs.previousInputState == INPUT_IS_NOT_PRESSED){
                    bs.currentInputState = INPUT_IS_NOT_PRESSED;
                }
                else if(bs.previousInputState == INPUT_IS_JUST_RELEASED){
                    bs.currentInputState = INPUT_IS_NOT_PRESSED;
                }
                else if(bs.previousInputState == INPUT_IS_PRESSED){
                    bs.currentInputState = INPUT_IS_JUST_RELEASED;
                }
            }
            inputs[i].buttonStates[j] = bs;
        }
    }
}