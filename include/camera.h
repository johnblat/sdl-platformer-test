#ifndef cam_h
#define cam_h
#include "position.h"
#include "input.h"
#include "velocity.h"
#include "collisions.h"

extern Position gCameraPosition;
extern Position gCenterScreen;
extern float gZoomAmount;
extern float gZoomSpeed;

void cam_input_zoom_System(flecs::iter &it, Input *inputs);
void cam_zoom_render_frame_start_System(flecs::iter &it);
void cam_input_camera_move_System(flecs::iter &it, Input *inputs);
v2d cam_util_world_position_to_camera_position(v2d w);
void
cam_set_position_based_on_velocity_System(flecs::iter &it, Position *positions, Velocity *velocities, CollisionResultPlatformPathFloorSensor *collision_results);
#endif