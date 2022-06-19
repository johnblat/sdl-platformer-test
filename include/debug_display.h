#ifndef dbg_display_h
#define dbg_display_h

#include "ray2d.h"
#include <vector>
#include "solid_rect.h"
#include "velocity.h"
#include "mouseState.h"


void renderRectangularObjectsSystem(flecs::iter &it, Position *positions, SolidRect *rectObjects);


void renderUnselectedPlatformNodeNodesSystem(flecs::iter &it, Position *positions, PlatformNodeCollection *PlatformNodeCollection);
void renderSensorsSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections, GroundMode *groundModes);

void setColorOnPncSelect(flecs::iter &it, PlatformNodeCollection *pncs);

void setColorOnPncDeselect(flecs::iter &it, PlatformNodeCollection *pncs);

void renderUncommitedLinesToPlaceSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates);

void renderSelectedPlatformNodeSystem(flecs::iter &it, SelectedForEditing *ss, Position *positions, PlatformNodeCollection *platformVertexCollections);

void renderSelectedPlatformNodeNodesSystem(flecs::iter &it, SelectedForEditingNode *sns, Position *positions, PlatformNodeCollection *platformVertexCollections);

void renderUnselectedPlatformNodeSystem(flecs::iter &it, Position *positions, PlatformNodeCollection *platformVertexCollections);


#endif