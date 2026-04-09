#include "scene.h"

#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <GL/gl.h>

static float estimate_radius(const char* obj_path, float scale) {

    float base = 1.0f;

    if      (strstr(obj_path, "BirchTree")) base = 0.2f;
    else if (strstr(obj_path, "DeadTree"))  base = 0.2f;
    else if (strstr(obj_path, "Bush"))      base = 0.2f;
    else if (strstr(obj_path, "Rock"))      base = 0.2f;

    return base * scale;
}

void init_scene(Scene* scene) {

    scene->uptime = 60.0;
    scene->num_loaded_models = 0;

    const char* obj_files[] = {
        "assets/models/BirchTree_1.obj",
        "assets/models/BirchTree_2.obj",
        "assets/models/BirchTree_3.obj",
        "assets/models/BirchTree_4.obj",
        "assets/models/BirchTree_5.obj",
        "assets/models/BirchTree_1.obj",
        "assets/models/BirchTree_3.obj",
        "assets/models/BirchTree_5.obj",
        "assets/models/Bush.obj",
        "assets/models/Bush_small.obj",
        "assets/models/Bush.obj",
        "assets/models/DeadTree_1.obj",
        "assets/models/DeadTree_2.obj",
        "assets/models/Rock_1.obj",
        "assets/models/Rock_2.obj",
        "assets/models/Rock_3.obj"
    };

    int num_files_to_load = sizeof(obj_files) / sizeof(obj_files[0]);

    for (int i = 0; i < num_files_to_load && i < MAX_MODELS; i++) {

        if (load_model(&scene->models[scene->num_loaded_models], obj_files[i])) {

            scene->num_loaded_models++;
            printf("[INFO] Successfully loaded: %s\n", obj_files[i]);
        } else {
            printf("[ERROR] Cannot load: %s\n", obj_files[i]);
        }
    }

    scene->num_objects = MAX_OBJECTS; 

    for (int i = 0; i < scene->num_objects; i++) {

        bool valid_pos = false;

        while (!valid_pos) {

            float rx = (rand() % 1400 / 10.0f) - 70.0f;
            float ry = (rand() % 1400 / 10.0f) - 70.0f;

            float dist_clearing = sqrt(rx*rx + ry*ry);
            float dist_lake = sqrt(pow(rx - LAKE_CENTER_X, 2) + pow(ry - LAKE_CENTER_Y, 2));

            if (dist_clearing > 20.0f && dist_lake > 24.0f) {

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

void update_scene(Scene* scene, double time) {
    scene->uptime += time;
}

float smoothstep(float edge0, float edge1, float x) {

    float t = (x - edge0) / (edge1 - edge0);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return t * t * (3.0f - 2.0f * t);
}


static int is_visible(const Camera* camera, float ox, float oy, float obj_radius) {

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

void render_scene(const Scene* scene, const Camera* camera) {

    glEnable(GL_COLOR_MATERIAL);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);

    // Ground coloring with gradient & generating the lake's hole

    float size = 70.0f;
    float step = 2.0f; 

    float lakeCenterX = LAKE_CENTER_X;
    float lakeCenterY = LAKE_CENTER_Y;
    float lakeRadius  = LAKE_RADIUS;
    float maxDepth = 4.0f;

    float clearingColor[3] = {0.45f, 0.4f,  0.25f};
    float forestColor[3]   = {0.02f, 0.12f, 0.02f}; 

    for (float x = -size; x < size; x += step) {

        glBegin(GL_QUAD_STRIP);

        for (float y = -size; y <= size; y += step) {
            
            float noise1 = sin(x * 0.15f + y * 0.1f) * 3.0f + cos(x * 0.3f - y * 0.2f) * 1.5f;
            float dist1 = sqrt(x*x + y*y) + noise1;
            float t1 = smoothstep(5.0f, 20.0f, dist1);

            float dist_lake1 = sqrt(pow(x - lakeCenterX, 2) + pow(y - lakeCenterY, 2));
            float z1 = 0.0f;

            if (dist_lake1 < lakeRadius) {

                float depth_factor = 1.0f - (dist_lake1 / lakeRadius); 
                z1 = -maxDepth * (depth_factor * depth_factor); 
            }
            
            float r1 = clearingColor[0] * (1.0f - t1) + forestColor[0] * t1;
            float g1 = clearingColor[1] * (1.0f - t1) + forestColor[1] * t1;
            float b1 = clearingColor[2] * (1.0f - t1) + forestColor[2] * t1;

            if (z1 < -0.1f) {
                r1 *= 0.6f; 
                g1 *= 0.6f; 
                b1 *= 0.8f; 
            } 

            glColor3f(r1, g1, b1);
            glNormal3f(0, 0, 1);
            glVertex3f(x, y, z1);
            
            float x2 = x + step;
            float noise2 = sin(x2 * 0.15f + y * 0.1f) * 3.0f + cos(x2 * 0.3f - y * 0.2f) * 1.5f;
            float dist2 = sqrt(x2*x2 + y*y) + noise2;
            float t2 = smoothstep(5.0f, 20.0f, dist2);

            float dist_lake2 = sqrt(pow(x2 - lakeCenterX, 2) + pow(y - lakeCenterY, 2));
            float z2 = 0.0f;

            if (dist_lake2 < lakeRadius) {

                float depth_factor = 1.0f - (dist_lake2 / lakeRadius);
                z2 = -maxDepth * (depth_factor * depth_factor);
            }

            float r2 = clearingColor[0] * (1.0f - t2) + forestColor[0] * t2;
            float g2 = clearingColor[1] * (1.0f - t2) + forestColor[1] * t2;
            float b2 = clearingColor[2] * (1.0f - t2) + forestColor[2] * t2;

           if (z2 < -0.1f) {
                r2 *= 0.6f; 
                g2 *= 0.6f; 
                b2 *= 0.8f;
            }

            glColor3f(r2, g2, b2);
            glVertex3f(x2, y, z2);
        }
        glEnd();
    }

    // Draw lake

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.1f, 0.4f, 0.7f, 0.85f); 

    float waterLevel = -0.5f; 
    float waterStep = 1.0f;

    for (float wx = lakeCenterX - lakeRadius; wx < lakeCenterX + lakeRadius; wx += waterStep) {

        glBegin(GL_QUAD_STRIP);

        for (float wy = lakeCenterY - lakeRadius; wy <= lakeCenterY + lakeRadius; wy += waterStep) {
            
            float d1 = sqrt(pow(wx - lakeCenterX, 2) + pow(wy - lakeCenterY, 2));
            float ripple1 = 0.0f;
            
            if (d1 < lakeRadius) {

                float damping1 = pow(1.0f - (d1 / lakeRadius), 2.0f); 
                ripple1 = damping1 * 0.15f * sin(d1 * 1.0f - scene->uptime * 1.5f);
            }

            glNormal3f(0, 0, 1);
            glVertex3f(wx, wy, waterLevel + ripple1);

            float x2 = wx + waterStep;
            float d2 = sqrt(pow(x2 - lakeCenterX, 2) + pow(wy - lakeCenterY, 2));
            float ripple2 = 0.0f;
            
            if (d2 < lakeRadius) {

                float damping2 = pow(1.0f - (d2 / lakeRadius), 2.0f);
                ripple2 = damping2 * 0.15f * sin(d2 * 1.0f - scene->uptime * 1.5f);
            }

            glVertex3f(x2, wy, waterLevel + ripple2);
        }
        glEnd();
    }
    glDisable(GL_BLEND);

    // Draw objects with LOD and frustum culling

    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < scene->num_objects; i++) {

        float ox = scene->objects[i].position.x;
        float oy = scene->objects[i].position.y;

        float dx = ox - camera->position.x;
        float dy = oy - camera->position.y;
        float dist = sqrtf(dx*dx + dy*dy);

        if (dist > LOD_SKIP_DIST) continue;

        if (!is_visible(camera, ox, oy, scene->objects[i].radius + 2.0f)) continue;

        if (dist > LOD_FULL_DIST) {

            float obj_radius = scene->objects[i].radius;
            if (obj_radius < 0.5f) continue;
        }

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

    glDisable(GL_COLOR_MATERIAL);
}