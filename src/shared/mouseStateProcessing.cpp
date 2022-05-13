#include "mouseState.h"
#include "render.h"
#include "window.h"
#include "camera.h"
#include "mouseStateProcessing.h"

void mouseStateSetter(MouseState &mouseState){
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


    mouseState.windowPosition.x = (float)x;
    mouseState.windowPosition.y = (float)y;
    mouseState.logicalPosition.x = lx;
    mouseState.logicalPosition.y = ly;
    mouseState.cameraAdjustedPosition.x = caX;
    mouseState.cameraAdjustedPosition.y = caY;

    mouseState.lmbPreviousState = mouseState.lmbCurrentState;
    mouseState.rmbPreviousState = mouseState.rmbCurrentState;

    if(buttons & SDL_BUTTON_LMASK){
        if(mouseState.lmbPreviousState == INPUT_IS_JUST_PRESSED){
            mouseState.lmbCurrentState = INPUT_IS_PRESSED;
        }
        else if(mouseState.lmbPreviousState == INPUT_IS_NOT_PRESSED){
            mouseState.lmbCurrentState = INPUT_IS_JUST_PRESSED;
        }
        else if(mouseState.lmbPreviousState == INPUT_IS_JUST_RELEASED){
            mouseState.lmbCurrentState = INPUT_IS_JUST_PRESSED;
        }
        else if(mouseState.lmbPreviousState == INPUT_IS_PRESSED){
            mouseState.lmbCurrentState = INPUT_IS_PRESSED;
        }
    }
    else {
        if(mouseState.lmbPreviousState == INPUT_IS_JUST_PRESSED){
            mouseState.lmbCurrentState = INPUT_IS_JUST_RELEASED;
        }
        else if(mouseState.lmbPreviousState == INPUT_IS_NOT_PRESSED){
            mouseState.lmbCurrentState = INPUT_IS_NOT_PRESSED;
        }
        else if(mouseState.lmbPreviousState == INPUT_IS_JUST_RELEASED){
            mouseState.lmbCurrentState = INPUT_IS_NOT_PRESSED;
        }
        else if(mouseState.lmbPreviousState == INPUT_IS_PRESSED){
            mouseState.lmbCurrentState = INPUT_IS_JUST_RELEASED;
        }
    }

    if(buttons & SDL_BUTTON_RMASK){
        if(mouseState.rmbPreviousState == INPUT_IS_JUST_PRESSED){
            mouseState.rmbCurrentState = INPUT_IS_PRESSED;
        }
        else if(mouseState.rmbPreviousState == INPUT_IS_NOT_PRESSED){
            mouseState.rmbCurrentState = INPUT_IS_JUST_PRESSED;
        }
        else if(mouseState.rmbPreviousState == INPUT_IS_JUST_RELEASED){
            mouseState.rmbCurrentState = INPUT_IS_JUST_PRESSED;
        }
        else if(mouseState.rmbPreviousState == INPUT_IS_PRESSED){
            mouseState.rmbCurrentState = INPUT_IS_PRESSED;
        }
    }
    else {
        if(mouseState.rmbPreviousState == INPUT_IS_JUST_PRESSED){
            mouseState.rmbCurrentState = INPUT_IS_JUST_RELEASED;
        }
        else if(mouseState.rmbPreviousState == INPUT_IS_NOT_PRESSED){
            mouseState.rmbCurrentState = INPUT_IS_NOT_PRESSED;
        }
        else if(mouseState.rmbPreviousState == INPUT_IS_JUST_RELEASED){
            mouseState.rmbCurrentState = INPUT_IS_NOT_PRESSED;
        }
        else if(mouseState.rmbPreviousState == INPUT_IS_PRESSED){
            mouseState.rmbCurrentState = INPUT_IS_JUST_RELEASED;
        }
    }


}



void mouesStatePlatformVerticesRemoveAll(flecs::world &ecs, MouseState &mouseState){
    if(mouseState.rmbCurrentState == INPUT_IS_JUST_RELEASED){
        // THIS IS WACKY.
        // TODO FIX ME
        auto f = ecs.filter<PlatformVertices>();
        ecs.defer_begin();
        f.each([&](flecs::entity e, PlatformVertices pvs){
            e.remove<PlatformVertices>();
        });
        ecs.defer_end();
    }
}



