#include "flecs.h"
#include "animation.h"
#include "position.h"

/**
 * @brief renders an animated sprite's current frame at the posisions specified
 * 
 * @param animatedSprites list of animated sprites
 * @param positions list of posisitions for all the animated sprites
 * @param numEntities number of iterations over the above arrays
 */
void anim_render_AnimatedSprites_System(flecs::iter &it, AnimatedSprite *animatedSprites, Position *positions, Angle *angles);