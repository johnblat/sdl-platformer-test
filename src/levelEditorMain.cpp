#include <SDL2/SDL.h>
#include "position.h"
#include "solid_rect.h"
#include "flecs.h"
#include "window.h"
#include "debug_display.h"
#include "ints.h"
#include "input.h"



struct MouseState {
    Position windowPosition;
    Position logicalPosition;
    InputState lmbCurrentState;
    InputState lmbPreviousState;
    InputState rmbCurrentState;
    InputState rmbPreviousState;
};

bool isMouseButtonJustPressed(int button);
bool isMouseButtonJustReleased(int button);
bool isMouseButtonPressed(int button);

void mouseEventHandler(SDL_Event &event, MouseState mouseState);

void mouseEventHandler(SDL_Event &event, MouseState mouseState){
    mouseState.lmbPreviousState = mouseState.lmbCurrentState;
    mouseState.rmbPreviousState = mouseState.rmbCurrentState;

    if(event.type == SDL_MOUSEBUTTONDOWN){
        if(event.button.button == SDL_BUTTON_LEFT){
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
        else if(event.button.button == SDL_BUTTON_RIGHT){
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
    }
    else if(event.type == SDL_MOUSEBUTTONUP) {

    }
}


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

    if()


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
    // main loop flag
    bool quit = false;

    // flecs
    flecs::world ecs;

    ecs.system<Position, SolidRect>()
        .kind(flecs::OnStore)
        .iter(renderRectangularObjectsSystem);

    ecs.system<Input>()
        .kind(flecs::PreUpdate)
        .iter(inputUpdateSystem);

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

    InputButtonState buttonStates[3];
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

    Input input;
    input.buttonStates = buttonStates;
    input.numButtomStates = 3;

    flecs::entity user;
    user.set<Input>(input);



    while(!quit){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                quit = true;
                break;
            }
        }
        u8 *keyStates = (u8 *)SDL_GetKeyboardState(nullptr);
        KeyboardState keyboardState{};
        keyboardState.keyStates = keyStates; 
    }
}