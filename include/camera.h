#ifndef cam_h
#define cam_h
#include "position.h"
#include "input.h"

extern Position gCameraPosition;
extern Position gCenterScreen;
extern float gZoomAmount;
extern float gZoomSpeed;

void cam_input_zoom_System(flecs::iter &it, Input *inputs);
void cam_zoom_render_frame_start_System(flecs::iter &it);
void cam_input_camera_move_System(flecs::iter &it, Input *inputs);
v2d cam_util_world_position_to_camera_position(v2d w);

#endif