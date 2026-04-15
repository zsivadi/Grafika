#include "forest.h"
#include "visibility.h"

#include <GL/gl.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

static float estimate_radius(const char* obj_path, float scale) {

    float base = 1.0f;

    if      (strstr(obj_path, "BirchTree")) base = 0.2f;
    else if (strstr(obj_path, "DeadTree"))  base = 0.2f;
    else if (strstr(obj_path, "Bush"))      base = 0.2f;
    else if (strstr(obj_path, "Rock"))      base = 0.2f;

    return base * scale;
}

void init_forest(Scene* scene, const char* obj_files[]) {

    scene->num_objects = MAX_OBJECTS; 

    for (int i = 0; i < scene->num_objects; i++) {

        bool valid_pos = false;

        while (!valid_pos) {

            float rx = (rand() % 1400 / 10.0f) - 70.0f;
            float ry = (rand() % 1400 / 10.0f) - 70.0f;

            float dist_clearing = sqrtf(rx*rx + ry*ry);
            float dist_lake     = sqrtf(powf(rx - LAKE_CENTER_X, 2) + powf(ry - LAKE_CENTER_Y, 2));
            float dist_campfire = sqrtf(powf(rx - CAMPFIRE_X, 2) + powf(ry - CAMPFIRE_Y, 2));

            if (dist_clearing > 20.0f && dist_lake > 24.0f && dist_campfire > 8.0f) {

                scene->objects[i].position.x = rx;
                scene->objects[i].position.y = ry;
                scene->objects[i].position.z = 0.0f; 

                scene->objects[i].scale = 1.1f + (rand() % 70 / 100.0f);

                if (scene->num_loaded_models > 0) {

                    int idx = rand() % scene->num_loaded_models;

                    scene->objects[i].model_index = idx;
                    scene->objects[i].radius = estimate_radius(obj_files[idx], scene->objects[i].scale);

                } else {

                    scene->objects[i].model_index = -1;
                    scene->objects[i].radius = 1.0f;
                }

                valid_pos = true;
            }
        }
    }
}

void render_forest(const Scene* scene, const Camera* camera) {

    for (int i = 0; i < scene->num_objects; i++) {

        float ox = scene->objects[i].position.x;
        float oy = scene->objects[i].position.y;

        float ddx  = ox - camera->position.x;
        float ddy  = oy - camera->position.y;

        float dist = sqrtf(ddx*ddx + ddy*ddy);


        if (dist > LOD_SKIP_DIST) continue;
        if (!is_visible(camera, ox, oy, scene->objects[i].radius + 2.0f)) continue;
        if (dist > LOD_FULL_DIST && scene->objects[i].radius < 0.5f) continue;

        glPushMatrix();

        glTranslatef(ox, oy, scene->objects[i].position.z);
        glScalef(scene->objects[i].scale, scene->objects[i].scale, scene->objects[i].scale);
        
        float rotation_angle = (ox * oy) * 100.0f;

        glRotatef(rotation_angle, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 

        if (scene->objects[i].model_index >= 0) {
            draw_model(&scene->models[scene->objects[i].model_index]);
        }
        
        glPopMatrix();
    }
}