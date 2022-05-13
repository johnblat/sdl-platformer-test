#ifndef animproch
#define animproch

#include "animation.h"
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "position.h"
#include "input.h"
#include "flecs.h"
#include "velocity.h"
#include "states.h"

extern Position gCameraPosition;
/**
 * @brief runs all the animations provided. Will advance frames as per the fps specified
 * 
 * @param animations 
 * @param numAnimations 
 * @param deltaTime 
 */
// void animationsAccumulationSystem(AnimatedSprite *animatedSprites, size_t numAnimations, float deltaTime);
void animationsAccumulationSystem(flecs::iter &it, AnimatedSprite *animatedSprites);


/**
 * @brief sets the current animation based on the key pressed. Can be used for testing
 * 
 * @param it 
 * @param animatedSprites 
 * @param keyStatesCollections 
 */
void KeyboardStateAnimationSetterSystem(flecs::iter &it, AnimatedSprite *animatedSprites, KeyboardState *keyStatesCollections);

/**
 * @brief sets animation according to the velocity of the entity
 * 
 * @param it 
 * @param animatedSprites 
 * @param velocities 
 */
void setAnimationBasedOnSpeedSystem(flecs::iter &it, AnimatedSprite *animatedSprites, Velocity *velocities, StateCurrPrev *states);

/**
 * @brief will flip the animated sprite based on the key that is pressed. intended to set the left and right facing directions
 * 
 * @param it 
 * @param animatedSprites 
 * @param keyStatesCollections 
 */
void InputFlipSystem(flecs::iter &it, AnimatedSprite *animatedSprites, Input *inputs);

/**
 * @brief Create a Animated Sprite object with the spriteSheetId
 * 
 * @param spriteSheetId the id of the sprite sheet this animatedSprite will reference
 * @return AnimatedSprite 
 */
AnimatedSprite createAnimatedSprite(u32 spriteSheetId);

/**
 * @brief creates a blank animation to the animated Sprite's animation list
 * 
 * @param animatedSprite 
 */
void addNewAnimationToAnimatedSprite(AnimatedSprite *animatedSprite);

/**
 * @brief Overwrites an animation in the animation list at animationIndex within the animatedsprite
 * 
 * @param animatedSprite the Animated sprite to modify
 * @param animationIndex the index in the animated sprite's animation list to overwrite
 * @param Animation the animation that will overwrite the animation at the animation index
 */
void overwriteAnimationOnAnimatedSprite(AnimatedSprite *animatedSprite, u32 animationIndex, Animation animation);


/**
 * @brief Will actually set the ms per frame as well. Will use the fps provided to calculate this along with the number of frames in the animatedSprite. Storing the fps so that user can see this when modifying the value.
 * 
 * @param animatedSprite The animated sprite to calculate the ms per frame
 * @param fps frames per second
 */
void setFpsForAnimatedSprite(AnimatedSprite *animatedSprite, float fps);

/**
 * @brief renders an animated sprite's current frame at the posisions specified
 * 
 * @param animatedSprites list of animated sprites
 * @param positions list of posisitions for all the animated sprites
 * @param numEntities number of iterations over the above arrays
 */
void renderingAnimatedSpritesSystem(flecs::iter &it, AnimatedSprite *animatedSprites, Position *positions);

/**
 * @brief Sets the current animation of animated sprite to the one that matches  the animationName. Crashes if animationName doesn't exist
 * 
 * @param as the animatedSprite to set current animation of
 * @param animationName the name of the new current animation
 */
void animatedSpritePlay(AnimatedSprite *as, std::string animationName);

/**
 * @brief just sets the currentFrame to 0. This can be very helpful for non looping animatins
 * 
 * @param animation 
 */
void restartAnimation(Animation *animation);

/**
 * @brief has the number of frames in animation been reached?
 * 
 */
bool isAnimationFinished(Animation *animation);

/**
 * @brief Create a Animation object
 * 
 * @param frameIndices the indices into a sprite sheet that will be the frames
 * @param FPS the frames per second
 * @param isLoop does the animation loop?
 * @param name name of the animation
 * @return Animation 
 */
Animation createAnimation(std::vector<u32> frameIndices, float FPS, bool isLoop, std::string name);

#endif