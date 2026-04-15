#include "interactions.h"

#include <math.h>
#include <stdio.h>

int scene_interact(Scene* scene, const Camera* camera, double* uptime) {

    float interact_dist = 2.0f;
    float cam_angle = (float)(camera->rotation.z * (float)M_PI / 180.0f);
    float look_x    = cosf(cam_angle);
    float look_y    = sinf(cam_angle);

    float cx = camera->position.x + look_x * (interact_dist * 0.5f);
    float cy = camera->position.y + look_y * (interact_dist * 0.5f);

    float dcx = cx - CAMPFIRE_X;
    float dcy = cy - CAMPFIRE_Y;

    if (sqrtf(dcx*dcx + dcy*dcy) < scene->campfire_radius + interact_dist) {

        scene->fire_lit = !scene->fire_lit;
        printf("[INFO] Campfire %s\n", scene->fire_lit ? "lit" : "extinguished");
        return 1;
    }

    double day_phase = fmod(*uptime, 120.0) / 120.0;
    int is_night = (day_phase < 0.20 || day_phase >= 0.80);

    if (is_night) {

        for (int i = 0; i < 3; i++) {

            float dtx = cx - scene->tent_positions[i].x;
            float dty = cy - scene->tent_positions[i].y;

            if (sqrtf(dtx*dtx + dty*dty) < scene->tent_radius + interact_dist) {

                *uptime = 60.0;
                printf("[INFO] Skipping to daytime...\n");
                return 2;
            }
        }
    }

    return 0;
}