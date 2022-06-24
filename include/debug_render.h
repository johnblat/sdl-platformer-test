#ifndef dbg_display_h
#define dbg_display_h

#include "ray2d.h"
#include <vector>
#include "solid_rect.h"
#include "velocity.h"
#include "mouseState.h"


void renderRectangularObjectsSystem(flecs::iter &it, Position *positions, SolidRect *rectObjects);


void renderUnselectedPlatformNodeNodesSystem(flecs::iter &it, Position *positions, PlatformPath *platformPath);

void renderSensorsSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections, GroundMode *groundModes);


void renderUncommitedLinesToPlaceSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates);

void renderSelectedPlatformNodeSystem(flecs::iter &it, SelectedForEditing *ss, Position *positions, PlatformPath *platformPaths);

void renderSelectedPlatformNodeNodesSystem(flecs::iter &it, SelectedForEditingNode *sns, Position *positions, PlatformPath *platformPaths);

void renderUnselectedPlatformNodeSystem(flecs::iter &it, Position *positions, PlatformPath *platformPaths);


#endif