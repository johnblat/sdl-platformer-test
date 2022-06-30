#include "flecs.h"
#include "animation.h"
#include "animation_util.h"
#include "state_util.h"

void anim_update_AnimatedSprite_accumulate_time_to_increment_frame_System(flecs::iter &it, AnimatedSprite *animatedSprites){

    for(auto i : it){
        u32 currentAnimation = animatedSprites[i].currentAnimation;

        // FIXME: ensure that the final frame gets played for the full amount of time rather than immediately jumping back to first frame
        if(anim_util_Animation_has_reached_final_frame(&animatedSprites[i].animations[currentAnimation])){
            if(animatedSprites[i].animations[currentAnimation].isLoop == false) continue;
            animatedSprites[i].animations[currentAnimation].currentFrame = 0;
        }

        animatedSprites[i].animations[currentAnimation].accumulator += it.delta_time();

        if(animatedSprites[i].animations[currentAnimation].accumulator > animatedSprites[i].animations[currentAnimation].msPerFrame){            
            animatedSprites[i].animations[currentAnimation].accumulator = 0.0f;
            animatedSprites[i].animations[currentAnimation].currentFrame++;

            
        }
    }
    
    
}

// purely for testing
void anim_update_KeyboardState_AnimatedSprite_set_animation_System(flecs::iter &it, AnimatedSprite *animatedSprites, KeyboardState *keyStatesCollections){
    for(auto i : it){
        u8 *keyStates = keyStatesCollections[i].keyStates;
        if(keyStates[SDL_SCANCODE_0]){
            anim_util_AnimatedSprite_play(&animatedSprites[i], "walk");
        } 
        if(keyStates[SDL_SCANCODE_1]){
            anim_util_AnimatedSprite_play(&animatedSprites[i], "run");

        } 
        if(keyStates[SDL_SCANCODE_2]){
            anim_util_AnimatedSprite_play(&animatedSprites[i], "stand-attack");
            anim_util_Animation_restart(&animatedSprites[i].animations[animatedSprites[i].currentAnimation]);
        }
        if(keyStates[SDL_SCANCODE_3]){
            anim_util_AnimatedSprite_play(&animatedSprites[i], "idle");
        }
    }
}


void anim_update_AnimatedSprite_flip_based_on_Input_System(flecs::iter &it, AnimatedSprite *animatedSprites, Input *inputs){
    for(auto i : it){

        if(Input_is_pressed(inputs[i], "left")){
            animatedSprites[i].flip = SDL_FLIP_HORIZONTAL;
        }
        else if(Input_is_pressed(inputs[i], "right")){
            animatedSprites[i].flip = SDL_FLIP_NONE;
        }
    }
}

void anim_update_AnimatedSprite_set_animation_based_on_speed_on_ground_System(flecs::iter &it, AnimatedSprite *animatedSprites, Velocity *velocities, StateCurrPrev *states){
    for(int i : it){
        // set animation
        
        if(states[i].currentState == STATE_ON_GROUND){
            if(velocities[i].x == 0){
            anim_util_AnimatedSprite_play(&animatedSprites[i], "idle");
            }
            else if(velocities[i].x < 5.0f && velocities[i].x > -5.0f){
                anim_util_AnimatedSprite_play(&animatedSprites[i], "walk");
            }
            else {
                anim_util_AnimatedSprite_play(&animatedSprites[i], "run");
            }
        }
        
    }
}


void anim_update_set_jump_animation_on_jump_input_System(flecs::iter &it, AnimatedSprite *animatedSprites, Input *inputs){
    for(u64 i : it){
        if(Input_is_just_pressed(inputs[i], "jump")){
            anim_util_AnimatedSprite_play(&animatedSprites[i], "jump");
        }
    }
}




// void anim_update_set_animation_on_player_enter_ground_state(flecs::iter &it, AnimatedSprite *animatedSprites, StateCurrPrev *states, ){

// }