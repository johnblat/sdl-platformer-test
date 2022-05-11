#include "mouseState.h"
#include "render.h"

void mouseStateSetter(MouseState &mouseState){
    u32 buttons;
    int x, y;
    float lx, ly;

    buttons = SDL_GetMouseState(&x, &y);

    SDL_RenderWindowToLogical(
        gRenderer,
        x,
        y,
        &lx,
        &ly
    );

    mouseState.windowPosition.x = (float)x;
    mouseState.windowPosition.y = (float)y;
    mouseState.logicalPosition.x = lx;
    mouseState.logicalPosition.y = ly;

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


void mouseStatePositionCreate(flecs::world &ecs, MouseState &mouseState){
    if(mouseState.lmbCurrentState == INPUT_IS_JUST_RELEASED){
        Position p = {mouseState.cameraAdjustedPosition.x, mouseState.cameraAdjustedPosition.y};
        Position pv;
        pv.x = p.x;
        pv.y = p.y;

        // THIS IS WACKY.
        // TODO FIX ME
        auto f = ecs.filter<PlatformVertices>();
        f.each([&](flecs::entity e, PlatformVertices &pvs){
            pvs.vals.push_back(pv);
        });
    }
}
