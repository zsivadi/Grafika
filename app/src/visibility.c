#include "visibility.h"

#include <math.h>

int is_visible(const Camera* camera, float ox, float oy, float obj_radius) {

    float cam_angle = (float)(camera->rotation.z * M_PI / 180.0);

    float fwd_x = cosf(cam_angle);
    float fwd_y = sinf(cam_angle);

    float dx = ox - camera->position.x;
    float dy = oy - camera->position.y;

    float dist = sqrtf(dx*dx + dy*dy);

    if (dist < obj_radius + 2.0f) return 1;

    float dot = (dx * fwd_x + dy * fwd_y) / dist;
    return dot > -0.3f;
}