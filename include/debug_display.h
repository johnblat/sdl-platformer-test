#ifndef dbg_display_h
#define dbg_display_h

void renderRectangularObjectsSystem(flecs::iter &it, SolidRect *rectObjects);
void renderRay2dCollectionsSystem(flecs::iter &it, Position *positions, std::vector<Ray2d> *ray2dCollections);

#endif