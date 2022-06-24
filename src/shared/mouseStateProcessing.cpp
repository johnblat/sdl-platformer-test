#include "mouseState.h"
#include "render.h"
#include "window.h"
#include "camera.h"
#include "mouseStateProcessing.h"

void MouseState_update_System(flecs::iter &it, MouseState *mouseStates){
    u32 buttons;
    int x, y;
    float lx, ly;
    float caX, caY;

    buttons = SDL_GetMouseState(&x, &y);

    SDL_RenderWindowToLogical(
        gRenderer,
        x,
        y,
        &lx,
        &ly
    );
    // if(buttons & SDL_BUTTON_LMASK){
    //     printf("hi");
    // }

    float scaleX, scaleY;
    SDL_RenderGetScale(gRenderer, &scaleX, &scaleY);

    float centerScreenX, centerScreenY;
    centerScreenX = gScreenWidth/2;
    centerScreenY = gScreenHeight/2;

    float scaledCenterScreenX, scaledCenterScreenY;
    scaledCenterScreenX = centerScreenX / scaleX;
    scaledCenterScreenY = centerScreenY / scaleY;



    caX = lx - (scaledCenterScreenX - gCameraPosition.x);
    caY = ly - (scaledCenterScreenY - gCameraPosition.y);

    for(int i : it){
        mouseStates[i].windowPosition.x = (float)x;
        mouseStates[i].windowPosition.y = (float)y;
        mouseStates[i].logicalPosition.x = lx;
        mouseStates[i].logicalPosition.y = ly;
        mouseStates[i].worldPosition.x = caX;
        mouseStates[i].worldPosition.y = caY;

        mouseStates[i].lmbPreviousState = mouseStates[i].lmbCurrentState;
        mouseStates[i].rmbPreviousState = mouseStates[i].rmbCurrentState;

        if(buttons & SDL_BUTTON_LMASK){
            if(mouseStates[i].lmbPreviousState == INPUT_IS_JUST_PRESSED){
                mouseStates[i].lmbCurrentState = INPUT_IS_PRESSED;
            }
            else if(mouseStates[i].lmbPreviousState == INPUT_IS_NOT_PRESSED){
                mouseStates[i].lmbCurrentState = INPUT_IS_JUST_PRESSED;
            }
            else if(mouseStates[i].lmbPreviousState == INPUT_IS_JUST_RELEASED){
                mouseStates[i].lmbCurrentState = INPUT_IS_JUST_PRESSED;
            }
            else if(mouseStates[i].lmbPreviousState == INPUT_IS_PRESSED){
                mouseStates[i].lmbCurrentState = INPUT_IS_PRESSED;
            }
        }
        else {
            if(mouseStates[i].lmbPreviousState == INPUT_IS_JUST_PRESSED){
                mouseStates[i].lmbCurrentState = INPUT_IS_JUST_RELEASED;
            }
            else if(mouseStates[i].lmbPreviousState == INPUT_IS_NOT_PRESSED){
                mouseStates[i].lmbCurrentState = INPUT_IS_NOT_PRESSED;
            }
            else if(mouseStates[i].lmbPreviousState == INPUT_IS_JUST_RELEASED){
                mouseStates[i].lmbCurrentState = INPUT_IS_NOT_PRESSED;
            }
            else if(mouseStates[i].lmbPreviousState == INPUT_IS_PRESSED){
                mouseStates[i].lmbCurrentState = INPUT_IS_JUST_RELEASED;
            }
        }

        if(buttons & SDL_BUTTON_RMASK){
            if(mouseStates[i].rmbPreviousState == INPUT_IS_JUST_PRESSED){
                mouseStates[i].rmbCurrentState = INPUT_IS_PRESSED;
            }
            else if(mouseStates[i].rmbPreviousState == INPUT_IS_NOT_PRESSED){
                mouseStates[i].rmbCurrentState = INPUT_IS_JUST_PRESSED;
            }
            else if(mouseStates[i].rmbPreviousState == INPUT_IS_JUST_RELEASED){
                mouseStates[i].rmbCurrentState = INPUT_IS_JUST_PRESSED;
            }
            else if(mouseStates[i].rmbPreviousState == INPUT_IS_PRESSED){
                mouseStates[i].rmbCurrentState = INPUT_IS_PRESSED;
            }
        }
        else {
            if(mouseStates[i].rmbPreviousState == INPUT_IS_JUST_PRESSED){
                mouseStates[i].rmbCurrentState = INPUT_IS_JUST_RELEASED;
            }
            else if(mouseStates[i].rmbPreviousState == INPUT_IS_NOT_PRESSED){
                mouseStates[i].rmbCurrentState = INPUT_IS_NOT_PRESSED;
            }
            else if(mouseStates[i].rmbPreviousState == INPUT_IS_JUST_RELEASED){
                mouseStates[i].rmbCurrentState = INPUT_IS_NOT_PRESSED;
            }
            else if(mouseStates[i].rmbPreviousState == INPUT_IS_PRESSED){
                mouseStates[i].rmbCurrentState = INPUT_IS_JUST_RELEASED;
            }
        }
    }

    


}






