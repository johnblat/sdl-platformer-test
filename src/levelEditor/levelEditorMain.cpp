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
#include "mouseStateProcessing.h"

struct MouseState {
    Position windowPosition;
    Position logicalPosition;
    InputState lmbCurrentState;
    InputState lmbPreviousState;
    InputState rmbCurrentState;
    InputState rmbPreviousState;
};

MouseState mouseState;





SDL_Renderer *gRenderer;
SDL_Window *gWindow;

int gScreenWidth = 640 * 2;
int gScreenHeight = 480 * 2;

SDL_Color bgColor = {20,20,20,255};




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

        mouseStateSetterSystem(mouseState);
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