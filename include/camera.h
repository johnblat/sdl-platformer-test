#ifndef cam_h
#define cam_h
#include "position.h"
#include "input.h"

extern Position gCameraPosition;
extern Position gCenterScreen;
extern float gZoomAmount;
extern float gZoomSpeed;

void inputZoomSystem(flecs::iter &it, Input *inputs);
void zoomRenderSetupSystem(flecs::iter &it);
void inputCameraMoveSystem(flecs::iter &it, Input *inputs);
v2d worldPositionToCamPosition(v2d w);

#endif