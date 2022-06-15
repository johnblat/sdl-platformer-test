#ifndef dbg_display_h
#define dbg_display_h

#include "ray2d.h"
#include <vector>
#include "solid_rect.h"
#include "velocity.h"
#include "mouseState.h"


void renderRectangularObjectsSystem(flecs::iter &it, Position *positions, SolidRect *rectObjects);


void renderUnselectedPlatformVerticesNodesSystem(flecs::iter &it, Position *positions, PlatformVertexCollection *PlatformVerticesCollection);
void renderSensorsSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections, GroundMode *groundModes);

void setColorOnPVCSelect(flecs::iter &it, PlatformVertexCollection *pvcs);

void setColorOnPVCDeselect(flecs::iter &it, PlatformVertexCollection *pvcs);

void renderUncommitedLinesToPlaceSystem(flecs::iter &it, Input *inputs, MouseState *mouseStates);

void renderSelectedPlatformVerticesSystem(flecs::iter &it, SelectedForEditing *ss, Position *positions, PlatformVertexCollection *platformVertexCollections);

void renderSelectedPlatformVerticesNodesSystem(flecs::iter &it, SelectedForEditingNode *sns, Position *positions, PlatformVertexCollection *platformVertexCollections);

void renderUnselectedPlatformVerticesSystem(flecs::iter &it, Position *positions, PlatformVertexCollection *platformVertexCollections);


#endif