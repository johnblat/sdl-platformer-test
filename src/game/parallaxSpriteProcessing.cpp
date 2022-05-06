#include "flecs.h"
#include "position.h"
#include "ParallaxSprite.h"
#include "parallaxSpriteProcessing.h"
#include "window.h"
#include "spriteSheetsProcessing.h"
#include "camera.h"
#include <SDL2/SDL.h>
#include "render.h"


void renderParallaxSpriteSystem(flecs::iter &it, Position *positions, ParallaxSprite *parallaxSprites){
    for(int i : it){
        Position centerScreen = {(float)gScreenWidth/2, (float)gScreenHeight/2};
        Position scaledCenterScreen = {centerScreen.x / gZoomAmount, centerScreen.y / gZoomAmount};

        SpriteSheet spriteSheet = getSpriteSheet(parallaxSprites[i].spriteSheetId);

        SDL_Rect destRect;
        destRect.x =
                (((int) positions[i].x - (int) gCameraPosition.x*parallaxSprites[i].scale) + (int) scaledCenterScreen.x -
                ((int) spriteSheet.w / 2));
        destRect.y =
                ((int) positions[i].y - (int) gCameraPosition.y * parallaxSprites[i].scale + (int) scaledCenterScreen.y - ((int) spriteSheet.h / 2) );
        destRect.w = spriteSheet.w;
        destRect.h = spriteSheet.h;

        SDL_RenderCopy(gRenderer, spriteSheet.texture, nullptr, &destRect);
    }
}