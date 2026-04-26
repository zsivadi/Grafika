#include "camera.h"
#include "terrain.h"

#include <GL/gl.h>
#include <math.h>

void init_camera(Camera* camera) {

    camera->position.x = 0.0;
    camera->position.y = 0.0;
    camera->position.z = 1.5;

    camera->rotation.x = 0.0;
    camera->rotation.y = 0.0;
    camera->rotation.z = 0.0;

    camera->speed.x = 0.0;
    camera->speed.y = 0.0;
    camera->speed.z = 0.0;

    camera->bobbing_timer = 0.0;
    camera->is_preview_visible = false;
}

void update_camera(Camera* camera, double time, const vec3* obstacles, const float* radii, int obstacle_count, float lake_x, float lake_y, float lake_radius, float fly_offset) {
    
    double angle;
    double side_angle;

    angle = degree_to_radian(camera->rotation.z);
    side_angle = degree_to_radian(camera->rotation.z + 90.0);

    float new_x = camera->position.x;
    float new_y = camera->position.y;

    new_x += (float)(cos(angle) * camera->speed.y * time);
    new_y += (float)(sin(angle) * camera->speed.y * time);
    new_x += (float)(cos(side_angle) * camera->speed.x * time);
    new_y += (float)(sin(side_angle) * camera->speed.x * time);

    float player_radius = PLAYER_HITBOX_RAD;
    int blocked = 0;

    // Check collision with obstacles (trees, rocks, etc.)
    
    for (int i = 0; i < obstacle_count; i++) {

        float dx = new_x - obstacles[i].x;
        float dy = new_y - obstacles[i].y;

        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < player_radius + radii[i]) {

            blocked = 1;
            break;
        }
    }

    // Check collision with lake boundary

    if (!blocked) {

        float dx = new_x - lake_x;
        float dy = new_y - lake_y;

        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < player_radius + lake_radius) {
            blocked = 1;
        }
    }

    // Update position if not blocked

    if (!blocked) {

        camera->position.x = new_x;
        camera->position.y = new_y;
    }

    // Get terrain height at camera position

    float terrain_height = get_terrain_height(camera->position.x, camera->position.y);
    
    // Camera eye height above ground

    float eye_height = PLAYER_EYE_HEIGHT;
    
    // Smoothly adjust camera height to follow terrain

    float target_z = terrain_height + eye_height + fly_offset;

    float height_lerp_factor = 10.0f * time; 
    if (height_lerp_factor > 1.0f) height_lerp_factor = 1.0f;
    
    camera->position.z += (target_z - camera->position.z) * height_lerp_factor;

    if (camera->speed.x != 0 || camera->speed.y != 0) {
        camera->bobbing_timer += time * 8.0;
    }
}

void set_view(const Camera* camera) {

    double bob_z = sin(camera->bobbing_timer) * 0.05;
    double bob_x = cos(camera->bobbing_timer * 0.5) * 0.02;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(-(camera->rotation.x + 90), 1.0, 0, 0);
    glRotatef(-(camera->rotation.z - 90), 0, 0, 1.0);

    glTranslatef(-camera->position.x + bob_x, -camera->position.y, -camera->position.z + bob_z);
}

void rotate_camera(Camera* camera, double horizontal, double vertical) {
    
    camera->rotation.z += horizontal;
    camera->rotation.x += vertical;

    if (camera->rotation.x > 89.0) camera->rotation.x = 89.0;
    if (camera->rotation.x < -89.0) camera->rotation.x = -89.0;

    if (camera->rotation.z < 0) camera->rotation.z += 360.0;
    if (camera->rotation.z > 360.0) camera->rotation.z -= 360.0;
}

void set_camera_speed(Camera* camera, double speed) {
    camera->speed.y = speed;
}

void set_camera_side_speed(Camera* camera, double speed) {
    camera->speed.x = speed;
}