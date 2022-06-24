#include "animation.h"
#include "flecs.h"
#include "position.h"
#include "spriteSheet_util.h"
#include "render.h"
#include "window.h"
#include "camera.h"
#include "util.h"

void anim_render_AnimatedSprites_System(flecs::iter &it, AnimatedSprite *animatedSprites, Position *positions, Angle *angles){
    for(auto i : it){
        SpriteSheet spriteSheet = SpriteSheet_util_get_by_id(animatedSprites[i].spriteSheetId);
        float cell_h = spriteSheet.h / spriteSheet.numCellRows;
        float cell_w = spriteSheet.w / spriteSheet.numCellCols;

        float scale;
        SDL_RenderGetScale(gRenderer,&scale, NULL );
        gCenterScreen = {(float)gScreenWidth/2.0f, (float)gScreenHeight/2.0f};
        Position scaledCenterScreen = {gCenterScreen.x / scale, gCenterScreen.y / scale};
        
        SDL_Rect renderRect;
        renderRect.x = (positions[i].x - gCameraPosition.x + scaledCenterScreen.x) - (cell_w*0.5f);
        renderRect.y = (positions[i].y - gCameraPosition.y + scaledCenterScreen.y) - (cell_h*0.5f);
        
        renderRect.w = cell_w;
        renderRect.h = cell_h;
        

        Animation currentAnimation = animatedSprites[i].animations[animatedSprites[i].currentAnimation];
        SDL_Rect srcRect;
        u32 spriteSheetCellIndex = currentAnimation.frameIndeces[currentAnimation.currentFrame];
        u32 cell_r;
        u32 cell_c;
        util_index_1d_to_2d(spriteSheetCellIndex, spriteSheet.numCellCols, &cell_r, &cell_c);
        srcRect.x = cell_c * cell_w;
        srcRect.y = cell_r * cell_h;
        srcRect.w = cell_w;
        srcRect.h = cell_h;
        SDL_RendererFlip flip = animatedSprites[i].flip == SDL_FLIP_HORIZONTAL ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE; 
        SDL_RenderCopyEx(gRenderer, spriteSheet.texture, &srcRect, &renderRect, -util_rads_to_degrees(angles[i].rads), NULL, flip);
    }
    
}