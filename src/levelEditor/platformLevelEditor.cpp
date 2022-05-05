#include <SDL2/SDL.h>
#include "flecs.h"
#include "position.h"
#include "input.h"
#include "render.h"
#include "debug_display.h"
#include "camera.h"
#define V2D_IMPLEMENTATION
#include "v2d.h"
#include "resourceLoading.h"
#include "mouseState.h"
#include "eventHandling.h"


SDL_Renderer *gRenderer;
SDL_Window *gWindow;


int gScreenWidth = 640 * 2;
int gScreenHeight = 480 * 2;

// main loop flag
bool quit = false;


/**
 * @brief MOUSE RELATED STUFF
 * 
 */


MouseState mouseState;


bool isMouseButtonJustPressed(int button);
bool isMouseButtonJustReleased(int button);
bool isMouseButtonPressed(int button);


// CALL THIS AFTER DONE EVENT POLLING


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

void mouseStatePlatformVertexCreate(flecs::world &ecs){
    if(mouseState.lmbCurrentState == INPUT_IS_JUST_RELEASED){
        Position p = {mouseState.cameraAdjustedPosition.x, mouseState.cameraAdjustedPosition.y};
        PlatformVertex pv;
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


void mouesStatePlatformVerticesRemoveAll(flecs::world &ecs){
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
/**
 * @brief END MOUSE RELATED STUFF
 * 
 */


/**
 * @brief EVENT PROCESSNIG
 * 
 */




void quitEventProcessor(SDL_Event &event){
    if(event.type == SDL_QUIT){
        quit = true;
    }
}


void mouseWheelStateEventProcessor(SDL_Event &event){
    if(event.type == SDL_MOUSEWHEEL){
        if(event.wheel.y > 0){
            mouseState.currentMouseWheelState = SCROLL_UP;
        }
        else if(event.wheel.y < 0){
            mouseState.currentMouseWheelState = SCROLL_DOWN;
        }
    }
}

/**
 * @brief END EVENT PROCESSNIG
 * 
 */


/** PLATFORM RELATED STUFF
 * 
 */


/**
 * @brief Creates an entity for the platform vertices
 * 
 * @param ecs 
 */
void createPlatformVerticesAtPosition(flecs::world &ecs, Position pos);

/**
 * @brief adds a platformVertex to the platformVertices vector
 * 
 * @param ecs 
 * @param platformVertices 
 * @param platformVertex 
 */
void addPlatformVertex(flecs::world &ecs, PlatformVertices &platformVertices, PlatformVertex platformVertex);

/**
 * @brief Removes platform vertices componet from the entity
 * 
 * @param ecs 
 * @param eid 
 */
void clearPlatformVertices(flecs::world &ecs, flecs::entity eid);

/** END PLATFORM RELATED STUFF
 * 
 */




void savePlatformVertices(flecs::world &ecs){    
    ecs.defer_begin();

    auto f = ecs.filter<Position, PlatformVertices>();

    f.iter([](flecs::iter &it,const Position *positions,const PlatformVertices *pvs){ 
        SDL_RWops *saveContext = SDL_RWFromFile("platformVertices", "wb");
        
        i32 numEntities = it.count();
        SDL_RWwrite(saveContext, &numEntities, sizeof(i32), 1);
        SDL_RWwrite(saveContext, positions, sizeof(Position), it.count());

        for(int i = 0; i < numEntities; i++){    
            size_t vectorSize = pvs[i].vals.size();
            SDL_RWwrite(saveContext, &vectorSize, sizeof(size_t), 1);
            const PlatformVertex *vectorData = pvs[i].vals.data();
            SDL_RWwrite(saveContext, vectorData, sizeof(PlatformVertex), vectorSize);
        }

        SDL_RWclose(saveContext);
    });

    ecs.defer_end();
};




void saveSystem(flecs::iter &it, Input *inputs){
    for(u64 i : it){
        if(inputIsJustReleased(inputs[i], "save")){
            flecs::world ecs = it.world();
            // auto q = ecs.query<Position, PlatformVertices>();
            savePlatformVertices(ecs);
            printf("SAVED!\n");
        }
    }
}


void loadInputSystem(flecs::iter &it, Input *inputs){
    for(u64 i : it){
        if(inputIsJustReleased(inputs[i], "load")){
            flecs::world ecs = it.world();
            loadPlatformVertices(ecs);
            printf("LOADED!\n");
        }
    }
}

/**
 * @brief Registers the initial systems
 * 
 * @param ecs 
 */
void registerSystems(flecs::world &ecs){
    ecs.system<Input>()
        .kind(flecs::PreUpdate)
        .iter(inputUpdateSystem);

    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(saveSystem);
    
    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(loadInputSystem);
    
    ecs.system<Position, PlatformVertices>()
        .kind(flecs::OnStore)
        .iter(renderPlatformVerticesSystem);
    
    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(inputZoomSystem);
    
    ecs.system<>()
        .kind(flecs::OnUpdate)
        .iter(zoomRenderSetupSystem);
    
    ecs.system<Input>()
        .kind(flecs::OnUpdate)
        .iter(inputCameraMoveSystem);
    
}






SDL_Color bgColor = {20,20,20,255};

int main(){
    gZoomAmount = 1.0f;

    Input userInput;

    userInput.buttonStates.push_back((InputButtonState){
        std::string("save"),
        SDL_SCANCODE_S,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    userInput.buttonStates.push_back((InputButtonState){
        std::string("load"),
        SDL_SCANCODE_L,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    userInput.buttonStates.push_back((InputButtonState){
        std::string("zoom-in"),
        SDL_SCANCODE_UP,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    userInput.buttonStates.push_back((InputButtonState){
        std::string("zoom-out"),
        SDL_SCANCODE_DOWN,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    userInput.buttonStates.push_back((InputButtonState){
        std::string("zoom-reset"),
        SDL_SCANCODE_R,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    userInput.buttonStates.push_back((InputButtonState){
        std::string("camera-pan-up"),
        SDL_SCANCODE_W,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    userInput.buttonStates.push_back((InputButtonState){
        std::string("camera-pan-down"),
        SDL_SCANCODE_S,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    userInput.buttonStates.push_back((InputButtonState){
        std::string("camera-pan-left"),
        SDL_SCANCODE_A,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });

    userInput.buttonStates.push_back((InputButtonState){
        std::string("camera-pan-right"),
        SDL_SCANCODE_D,
        INPUT_IS_NOT_PRESSED,
        INPUT_IS_NOT_PRESSED
    });


    gCameraPosition.x = gScreenWidth/2;
    gCameraPosition.y = gScreenHeight/2;

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

    mouseState.lmbCurrentState = INPUT_IS_NOT_PRESSED;
    mouseState.rmbCurrentState = INPUT_IS_NOT_PRESSED;


    SDL_Event event;
    registerEventProcessor(quitEventProcessor);
    registerEventProcessor(mouseWheelStateEventProcessor);
    

    // flecs
    flecs::world ecs;

    registerSystems(ecs);

    flecs::entity editorUser = ecs.entity();
    editorUser.set<Input>(userInput);

    PlatformVertices pvs;
    pvs.color.r = 255;
    pvs.color.g = 255;
    pvs.color.b = 255;
    pvs.color.a = 255;

    flecs::entity pvsetity = ecs.entity();
    pvsetity.set<PlatformVertices>(pvs);
    pvsetity.set<Position>((Position){0,0});

    const float FPS = 60;
    const float secondsPerFrame = 1.0f / FPS;

    while(!quit){
        u64 startTicks = SDL_GetTicks();

        mouseState.currentMouseWheelState = NOT_SCROLLING;

        while(SDL_PollEvent(&event)){
            runRegisteredEventProcessors(event);
        }
        
        if(mouseState.currentMouseWheelState == SCROLL_UP){
            gZoomAmount += 0.05;
        }
        else if(mouseState.currentMouseWheelState == SCROLL_DOWN){
            gZoomAmount -= 0.05;
        }
        

        gKeyStates = (u8 *)SDL_GetKeyboardState(NULL);

        mouseStateSetter(mouseState);
        mouseStatePlatformVertexCreate(ecs);

        SDL_SetRenderDrawColor(gRenderer, bgColor.r, bgColor.b, bgColor.g, 255);
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