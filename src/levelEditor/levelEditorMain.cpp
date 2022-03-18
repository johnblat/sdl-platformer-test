#include <SDL2/SDL.h>
#include "position.h"
#include "solid_rect.h"
#include "flecs.h"
#include "window.h"
#include "render.h"
#include "debug_display.h"
#include "ints.h"
#include "input.h"
#include "camera.h"
#define V2D_IMPLEMENTATION
#include "v2d.h"


struct MouseState {
    Position windowPosition;
    Position logicalPosition;
    InputState lmbCurrentState;
    InputState lmbPreviousState;
    InputState rmbCurrentState;
    InputState rmbPreviousState;
};

MouseState mouseState;


bool isMouseButtonJustPressed(int button);
bool isMouseButtonJustReleased(int button);
bool isMouseButtonPressed(int button);


// CALL THIS AFTER DONE EVENT POLLING
void mouseStateSetter(MouseState &mouseState);

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


flecs::entity gTestRectEntity;

struct Selected {};

void InputSolidRectModificationSystem(flecs::iter &it, Input *inputs){
    for(int i : it){
        

        if(inputIsPressed(inputs[i], "rotate")){
            // turn CCW
            if(inputIsPressed(inputs[i], "left")){
                auto f = it.world().filter<SolidRect, Selected>();
                f.each([&](flecs::entity e, SolidRect &sr, Selected &s){
                    sr.rotation -= 0.05;
                });
            }
            // turn CW
            else if(inputIsPressed(inputs[i], "right")){
                auto f = it.world().filter<SolidRect, Selected>();
                f.each([&](flecs::entity e, SolidRect &sr, Selected &s){
                    sr.rotation += 0.05;
                });
            }
        }
        else if(inputIsPressed(inputs[i], "left")){
            auto f = it.world().filter<SolidRect, Selected>();
            f.each([&](flecs::entity e, SolidRect &sr, Selected &s){
                sr.w -= 1;
            });
        }
        else if(inputIsPressed(inputs[i], "right")){
            auto f = it.world().filter<SolidRect, Selected>();
            f.each([&](flecs::entity e, SolidRect &sr, Selected &s){
                sr.w += 1;
            });
        }
        else if(inputIsPressed(inputs[i], "up")){
            auto f = it.world().filter<SolidRect, Selected>();
            f.each([&](flecs::entity e, SolidRect &sr, Selected &s){
                sr.h += 1;
            });
        }
        else if(inputIsPressed(inputs[i], "down")){
            auto f = it.world().filter<SolidRect, Selected>();
            f.each([&](flecs::entity e, SolidRect &sr, Selected &s){
                sr.h -= 1;
            });
        }
    }
}


void mouseStateSolidRectCreator(flecs::world &ecs);
void mouseStateSolidRectCreator(flecs::world &ecs){
    if(mouseState.lmbCurrentState == INPUT_IS_JUST_RELEASED){
        Position p = {mouseState.logicalPosition.x, mouseState.logicalPosition.y};
        SolidRect sr = {
            50,50,
            0,
            (SDL_Color){255,0,0,255}
        };

        auto f = ecs.filter<Selected>();
        ecs.defer_begin();
        f.each([&](flecs::entity e, Selected &s){
            e.remove<Selected>();
        });
        ecs.defer_end();


        flecs::entity e = ecs.entity();
        e.set<SolidRect>(sr);
        e.set<Position>(p);
        e.add<Selected>();
        
    }
}


SDL_Renderer *gRenderer;
SDL_Window *gWindow;

int gScreenWidth = 640 * 2;
int gScreenHeight = 480 * 2;

SDL_Color bgColor = {20,20,20,255};



void createSolidRect(flecs::world &ecs, Position p, float w, float h);
void extendSolidRectWidthFromCenter(SolidRect &sr, float amount);
void extendSolidRectWidthOnLeft(SolidRect &sr, float amount);
void extendSolidRectWidthOnRight(SolidRect &sr, float amount);
void extendSolidRectHeightFromCenter(SolidRect &sr, float amount);
void extendSolidRectHeightOnTop(SolidRect &sr, float amount);
void extendSolidRectHeightFromBottom(SolidRect &sr, float amount);
void rotateSolidRectCCW(SolidRect &sr, float amountInRads);
void rotateSolidRectCW(SolidRect &sr, float amountInRads);


int main(){
    gCameraPosition.x = gScreenWidth/2;
    gCameraPosition.y = gScreenHeight/2;

    // main loop flag
    bool quit = false;

    // flecs
    flecs::world ecs;

    gTestRectEntity = ecs.entity("rect");

    ecs.system<Position, SolidRect>()
        .kind(flecs::OnStore)
        .iter(renderRectangularObjectsSystem);

    ecs.system<Input>()
        .kind(flecs::PreUpdate)
        .iter(inputUpdateSystem);
        
    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(InputSolidRectModificationSystem);

    // sdl
    SDL_Init(SDL_INIT_VIDEO);
    gWindow = SDL_CreateWindow(
        "title",
        0,
        0, 
        gScreenWidth,
        gScreenHeight, 
        SDL_WINDOW_SHOWN
    );
    gRenderer = SDL_CreateRenderer(
        gWindow, 
        -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC 
    );

    SDL_Event event;

    // inputs

    InputButtonState buttonStates[5];
    buttonStates[0] = createbuttonState(
        "rotate",
        SDL_SCANCODE_R
    );
    buttonStates[1] = createbuttonState(
        "left",
        SDL_SCANCODE_LEFT
    );
    buttonStates[2] = createbuttonState(
        "right",
        SDL_SCANCODE_RIGHT
    );
    buttonStates[3] = createbuttonState(
        "up",
        SDL_SCANCODE_UP
    );
    buttonStates[4] = createbuttonState(
        "down",
        SDL_SCANCODE_DOWN
    );

    Input input;
    input.buttonStates = buttonStates;
    input.numButtomStates = 5;

    flecs::entity user = ecs.entity("user");
    user.set<Input>(input);

    mouseState.lmbCurrentState = INPUT_IS_NOT_PRESSED;
    mouseState.rmbCurrentState = INPUT_IS_NOT_PRESSED;

    
    gTestRectEntity.set<SolidRect>({20.0f,20.0f,0.0f, (SDL_Color){255,0,0,255}});
    gTestRectEntity.set<Position>({gScreenWidth/2.0f, gScreenHeight/2.0f});
    gTestRectEntity.add<Selected>();


    const float FPS = 60;
    const float secondsPerFrame = 1.0f / FPS;
    
    
    while(!quit){
        u64 startTicks = SDL_GetTicks();


        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
                break;
            }
        }
        gKeyStates = (u8 *)SDL_GetKeyboardState(nullptr);

        mouseStateSetter(mouseState);
        mouseStateSolidRectCreator(ecs);

        SDL_SetRenderDrawColor(gRenderer, bgColor.r, bgColor.g, bgColor.b, 255);
        SDL_RenderClear(gRenderer);

        ecs.progress();

        SDL_RenderPresent(gRenderer);

        u64 endTicks = SDL_GetTicks();

        u64 totalTicks = endTicks - startTicks;
        float totalSeconds = (float)totalTicks / 1000.0f;

        if(totalSeconds < secondsPerFrame){
            float secondsRemainingToFixTimeStep = secondsPerFrame - totalSeconds;
            float msRemainingToFixTimeStep = secondsRemainingToFixTimeStep * 1000;
            SDL_Delay((u32)msRemainingToFixTimeStep);
            //printf("ms to wait: %f\n", msRemainingToFixTimeStep);
        }

    }
}