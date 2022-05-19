#ifndef dbg_display_h
#define dbg_display_h

#include "ray2d.h"
#include <vector>
#include "solid_rect.h"

void renderRectangularObjectsSystem(flecs::iter &it, Position *positions, SolidRect *rectObjects);
void renderSensorsSystem(flecs::iter &it, Position *positions, Sensors *sensorCollections);

void renderPlatformVerticesSystem(flecs::iter &it, Position *positions, PlatformVertices *platformVerticesCollection);
void renderPlatformVerticesNodesSystem(flecs::iter &it, Position *positions, PlatformVertices *PlatformVerticesCollection);
#endif