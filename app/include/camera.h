#ifndef CAMERA_H
#define CAMERA_H

#include "utils.h"

#include <stdbool.h>


// Camera structure representing the player's viewpoint and movement

typedef struct Camera {
    vec3 position;
    vec3 rotation;
    vec3 speed;
    bool is_preview_visible;
    double bobbing_timer;
} Camera;

// Initialize the camera to the starting position

void init_camera(Camera* camera);

// Update camera position based on time, collision detection with obstacles and lake boundaries

void update_camera(Camera* camera, double time, const vec3* obstacles, const float* radii, int obstacle_count, float lake_x, float lake_y, float lake_radius, float fly_offset);

// Apply camera transformation to the OpenGL view matrix

void set_view(const Camera* camera);

// Rotate camera view by horizontal (yaw) and vertical (pitch) angles in degrees

void rotate_camera(Camera* camera, double horizontal, double vertical);

// Set forward/backward movement speed

void set_camera_speed(Camera* camera, double speed);

// Set left/right strafe movement speed

void set_camera_side_speed(Camera* camera, double speed);

#endif /* CAMERA_H */