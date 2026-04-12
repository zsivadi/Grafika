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


// Duck movement controller
// If the duck hit the edge of the lake it will change to a random direction

static void duck_random_dir(Duck* duck) {

    float dx = duck->x - LAKE_CENTER_X;
    float dy = duck->y - LAKE_CENTER_Y;
    float dist = sqrtf(dx*dx + dy*dy);

    float angle = ((float)(rand() % 1000) / 1000.0f) * 2.0f * (float)M_PI;
    float nx = cosf(angle);
    float ny = sinf(angle);

    if (dist > LAKE_RADIUS * 0.6f) {

        float inward_x = -dx / dist;
        float inward_y = -dy / dist;

        float blend = ((float)(rand() % 1000) / 1000.0f - 0.5f);
        nx = inward_x + blend * inward_y;
        ny = inward_y - blend * inward_x;
    }

    float len = sqrtf(nx*nx + ny*ny);
    if (len < 0.001f) len = 1.0f;
    duck->dir_x = nx / len;
    duck->dir_y = ny / len;
}

void init_scene(Scene* scene) {

    scene->uptime = 60.0;
    scene->num_loaded_models = 0;
    scene->fire_lit = 1;
    scene->fire_brightness = 1.0f;

    const char* obj_files[] = {
        "assets/models/BirchTree_1.obj",
        "assets/models/BirchTree_1.obj",
        "assets/models/BirchTree_2.obj",
        "assets/models/BirchTree_3.obj",
        "assets/models/BirchTree_3.obj",
        "assets/models/BirchTree_4.obj",
        "assets/models/BirchTree_5.obj",
        "assets/models/BirchTree_5.obj",
        "assets/models/DeadTree_1.obj",
        "assets/models/DeadTree_2.obj",
        "assets/models/Rock_1.obj",
        "assets/models/Rock_2.obj",
        "assets/models/Rock_3.obj"
        "assets/models/Bush.obj",
        "assets/models/Bush.obj",
        "assets/models/Bush.obj",
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

    load_model(&scene->campfire_model, "assets/models/Campfire.obj");
    load_model(&scene->tent_model,     "assets/models/Tent.obj");
    load_model(&scene->duck_model,     "assets/models/MallardDuck.obj");

    scene->campfire_radius = 0.5f;
    scene->tent_radius     = 1.0f;      
    float tent_dist = 5.0f;

    for (int i = 0; i < 3; i++) {

        float a = (float)i * (2.0f * (float)M_PI / 3.0f);
        scene->tent_positions[i].x = CAMPFIRE_X + cosf(a) * tent_dist;
        scene->tent_positions[i].y = CAMPFIRE_Y + sinf(a) * tent_dist;
        scene->tent_positions[i].z = 0.0f;
    }

    // Spawning the ducks

    for (int d = 0; d < 2; d++) {

        float start_angle = (float)d * (float)M_PI;
        scene->ducks[d].x = LAKE_CENTER_X + cosf(start_angle) * (LAKE_RADIUS * 0.5f);
        scene->ducks[d].y = LAKE_CENTER_Y + sinf(start_angle) * (LAKE_RADIUS * 0.5f);
        duck_random_dir(&scene->ducks[d]);
    }

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

void update_scene(Scene* scene, double time) {

    scene->uptime += time;
    float duck_speed = 1.2f;

    for (int d = 0; d < 2; d++) {

        Duck* duck = &scene->ducks[d];

        duck->x += duck->dir_x * duck_speed * (float)time;
        duck->y += duck->dir_y * duck_speed * (float)time;

        float dx = duck->x - LAKE_CENTER_X;
        float dy = duck->y - LAKE_CENTER_Y;
        float dist = sqrtf(dx*dx + dy*dy);

        // The edge of the lake

        if (dist >= LAKE_RADIUS * 0.55f) {

            float nd = LAKE_RADIUS * 0.55f;
            duck->x = LAKE_CENTER_X + (dx / dist) * nd;
            duck->y = LAKE_CENTER_Y + (dy / dist) * nd;

            duck_random_dir(duck);
        }
    }
}

float smoothstep(float edge0, float edge1, float x) {

    float t = (x - edge0) / (edge1 - edge0);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return t * t * (3.0f - 2.0f * t);
}

// LOD

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

// Drawing the fire of the campfire

static void draw_fire(double uptime, float cam_x, float cam_y, float fire_x, float fire_y) {

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDepthMask(GL_FALSE);

    int num_particles = 18;

    for (int i = 0; i < num_particles; i++) {

        float phase   = (float)i / num_particles;
        float t       = (float)fmod(uptime * 1.8 + phase, 1.0);
        float flicker = (float)(sin(uptime * 12.0 + i * 2.3) * 0.1 + 0.9);

        float px = sinf(phase * 6.28f + (float)uptime * 0.7f) * 0.25f * (1.0f - t);
        float py = cosf(phase * 4.71f + (float)uptime * 0.5f) * 0.25f * (1.0f - t);
        float pz = t * 1.8f;

        float size  = (0.35f - t * 0.25f) * flicker;
        float alpha = (1.0f - t) * 0.45f * flicker;  

        float r = 1.0f;
        float g = 0.4f + (1.0f - t) * 0.4f;
        float b = t * 0.05f;

        // The fire always face the camera

        float to_cam_x = cam_x - fire_x;
        float to_cam_y = cam_y - fire_y;
        float tc_len = sqrtf(to_cam_x*to_cam_x + to_cam_y*to_cam_y);

        if (tc_len < 0.001f) tc_len = 1.0f;

        float right_x = -to_cam_y / tc_len;
        float right_y =  to_cam_x / tc_len;

        glColor4f(r, g, b, alpha);
        glBegin(GL_QUADS);
            glVertex3f(px + right_x * (-size),       py + right_y * (-size),       pz);
            glVertex3f(px + right_x * ( size),       py + right_y * ( size),       pz);
            glVertex3f(px + right_x * ( size*0.5f),  py + right_y * ( size*0.5f),  pz + size * 2.0f);
            glVertex3f(px + right_x * (-size*0.5f),  py + right_y * (-size*0.5f),  pz + size * 2.0f);
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

// Drawing the smoke of the campfire

static void draw_smoke(double uptime, float cam_x, float cam_y, float fire_x, float fire_y) {

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    int num_puffs = 8;

    for (int i = 0; i < num_puffs; i++) {

        float phase = (float)i / num_puffs;
        float t     = (float)fmod(uptime * 0.4 + phase, 1.0);

        float px = sinf(phase * 5.1f + (float)uptime * 0.2f) * 0.15f * t;
        float py = cosf(phase * 3.7f + (float)uptime * 0.15f) * 0.15f * t;
        float pz = 1.8f + t * 3.5f;

        float size  = 0.2f + t * 0.8f;
        float alpha = (1.0f - t) * 0.12f;  
        float grey  = 0.5f + t * 0.2f;

        float to_cam_x = cam_x - fire_x;
        float to_cam_y = cam_y - fire_y;
        float tc_len = sqrtf(to_cam_x*to_cam_x + to_cam_y*to_cam_y);

        if (tc_len < 0.001f) tc_len = 1.0f;

        float rx = -to_cam_y / tc_len;
        float ry =  to_cam_x / tc_len;

        glColor4f(grey, grey, grey, alpha);
        glBegin(GL_QUADS);
            glVertex3f(px + rx*(-size), py + ry*(-size), pz);
            glVertex3f(px + rx*( size), py + ry*( size), pz);
            glVertex3f(px + rx*( size), py + ry*( size), pz + size*1.5f);
            glVertex3f(px + rx*(-size), py + ry*(-size), pz + size*1.5f);
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
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
    float maxDepth    = 4.0f;

    float clearingColor[3] = {0.45f, 0.4f,  0.25f};
    float forestColor[3]   = {0.02f, 0.12f, 0.02f}; 

    for (float x = -size; x < size; x += step) {

        glBegin(GL_QUAD_STRIP);

        for (float y = -size; y <= size; y += step) {
            
            float noise1 = sin(x * 0.15f + y * 0.1f) * 3.0f + cos(x * 0.3f - y * 0.2f) * 1.5f;
            float dist1  = sqrt(x*x + y*y) + noise1;
            float t1     = smoothstep(5.0f, 20.0f, dist1);

            float dist_lake1 = sqrt(pow(x - lakeCenterX, 2) + pow(y - lakeCenterY, 2));
            float z1 = 0.0f;

            if (dist_lake1 < lakeRadius) {

                float depth_factor = 1.0f - (dist_lake1 / lakeRadius); 
                z1 = -maxDepth * (depth_factor * depth_factor); 
            }
            
            float r1 = clearingColor[0] * (1.0f - t1) + forestColor[0] * t1;
            float g1 = clearingColor[1] * (1.0f - t1) + forestColor[1] * t1;
            float b1 = clearingColor[2] * (1.0f - t1) + forestColor[2] * t1;

            if (z1 < -0.1f) { r1 *= 0.6f; g1 *= 0.6f; b1 *= 0.8f; }

            glColor3f(r1, g1, b1);
            glNormal3f(0, 0, 1);
            glVertex3f(x, y, z1);
            
            float x2     = x + step;
            float noise2 = sin(x2 * 0.15f + y * 0.1f) * 3.0f + cos(x2 * 0.3f - y * 0.2f) * 1.5f;
            float dist2  = sqrt(x2*x2 + y*y) + noise2;
            float t2     = smoothstep(5.0f, 20.0f, dist2);

            float dist_lake2 = sqrt(pow(x2 - lakeCenterX, 2) + pow(y - lakeCenterY, 2));
            float z2 = 0.0f;

            if (dist_lake2 < lakeRadius) {
                float depth_factor = 1.0f - (dist_lake2 / lakeRadius);
                z2 = -maxDepth * (depth_factor * depth_factor);
            }

            float r2 = clearingColor[0] * (1.0f - t2) + forestColor[0] * t2;
            float g2 = clearingColor[1] * (1.0f - t2) + forestColor[1] * t2;
            float b2 = clearingColor[2] * (1.0f - t2) + forestColor[2] * t2;

            if (z2 < -0.1f) { r2 *= 0.6f; g2 *= 0.6f; b2 *= 0.8f; }

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
    float waterStep  = 1.0f;

    for (float wx = lakeCenterX - lakeRadius; wx < lakeCenterX + lakeRadius; wx += waterStep) {

        glBegin(GL_QUAD_STRIP);

        for (float wy = lakeCenterY - lakeRadius; wy <= lakeCenterY + lakeRadius; wy += waterStep) {
            
            float d1      = sqrt(pow(wx - lakeCenterX, 2) + pow(wy - lakeCenterY, 2));
            float ripple1 = 0.0f;
            
            if (d1 < lakeRadius) {
                float damping1 = pow(1.0f - (d1 / lakeRadius), 2.0f); 
                ripple1 = damping1 * 0.15f * sin(d1 * 1.0f - scene->uptime * 1.5f);
            }

            glNormal3f(0, 0, 1);
            glVertex3f(wx, wy, waterLevel + ripple1);

            float x2  = wx + waterStep;
            float d2  = sqrt(pow(x2 - lakeCenterX, 2) + pow(wy - lakeCenterY, 2));
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


    // Draw tents 

    glEnable(GL_TEXTURE_2D);
    for (int i = 0; i < 3; i++) {

        glPushMatrix();
        glTranslatef(scene->tent_positions[i].x, scene->tent_positions[i].y, 0.35f);

        float angle_to_fire = atan2f(CAMPFIRE_Y - scene->tent_positions[i].y,
                                     CAMPFIRE_X - scene->tent_positions[i].x);
        glRotatef(angle_to_fire * 180.0f / (float)M_PI + 90.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(3.0f, 3.0f, 1.5f);

        draw_model(&scene->tent_model);
        glPopMatrix();
    }


    // Draw campfire 

    glPushMatrix();
    glTranslatef(CAMPFIRE_X, CAMPFIRE_Y, 0.2f);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    draw_model(&scene->campfire_model);
    glPopMatrix();


    // Checking if the fire is lit

    if (scene->fire_lit) {

        float flicker      = (float)(sin(scene->uptime * 13.7) * 0.15 + 0.85);
        float brightness   = scene->fire_brightness * flicker;
        float fire_diffuse[] = { 1.0f * brightness, 0.45f * brightness, 0.05f * brightness, 1.0f };
        float fire_pos[]     = { CAMPFIRE_X, CAMPFIRE_Y, 0.8f, 1.0f };

        glEnable(GL_LIGHT1);
        glLightfv(GL_LIGHT1, GL_DIFFUSE,  fire_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, fire_diffuse);
        glLightfv(GL_LIGHT1, GL_POSITION, fire_pos);
        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.18f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.08f);

    } else {
        glDisable(GL_LIGHT1);
    }


    // Draw ducks

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    for (int d = 0; d < 2; d++) {

        const Duck* duck = &scene->ducks[d];
        float ddx = duck->x - LAKE_CENTER_X;
        float ddy = duck->y - LAKE_CENTER_Y;

        if (sqrtf(ddx*ddx + ddy*ddy) > LAKE_RADIUS * 0.75f) continue;

        glPushMatrix();
        glTranslatef(duck->x, duck->y, waterLevel - 0.35f);

        float heading = atan2f(duck->dir_y, duck->dir_x);
        glRotatef(heading * 180.0f / (float)M_PI + 90.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(0.5f, 0.5f, 0.5f);

        draw_model(&scene->duck_model);
        glPopMatrix();
    }

    // Drawing forest objects with LOD and frustum culling

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

    if (scene->fire_lit) {
        
        glPushMatrix();
            glTranslatef(CAMPFIRE_X, CAMPFIRE_Y, 0.0f);
            draw_fire(scene->uptime, camera->position.x, camera->position.y, CAMPFIRE_X, CAMPFIRE_Y);
            draw_smoke(scene->uptime, camera->position.x, camera->position.y, CAMPFIRE_X, CAMPFIRE_Y);
        glPopMatrix();
    }

    glDisable(GL_COLOR_MATERIAL);
}

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
                printf("[INFO] Skipping to daytime\n");
                return 2;
            }
        }
    }
    return 0;
}