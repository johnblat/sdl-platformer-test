#ifndef cam_h
#define cam_h
#include "position.h"

extern Position gCameraPosition;
extern Position gCenterScreen;
float gZoomAmount;
float gZoomSpeed;

void inputZoomSystem(flecs::iter &it, Input *inputs);
#endif