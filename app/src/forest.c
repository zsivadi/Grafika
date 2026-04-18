#include "forest.h"
#include "visibility.h"
#include "terrain.h"
#include "lake.h"

#include <GL/gl.h>
#include <math.h>

#include <stdlib.h>

void init_forest_chunk(Chunk* chunk) {
    chunk->num_objects = 0; 
    float start_x = chunk->cx * CHUNK_SIZE;
    float start_y = chunk->cy * CHUNK_SIZE;

    int attempts = 0;

    while (chunk->num_objects < MAX_CHUNK_OBJECTS && attempts < 500) {

        attempts++;

        float rx = start_x + ((float)rand() / RAND_MAX) * CHUNK_SIZE;
        float ry = start_y + ((float)rand() / RAND_MAX) * CHUNK_SIZE;

        float dist_clearing = sqrtf(rx*rx + ry*ry);
        float dist_lake     = sqrtf(powf(rx - LAKE_CENTER_X, 2) + powf(ry - LAKE_CENTER_Y, 2));
        float dist_campfire = sqrtf(powf(rx - CAMPFIRE_X, 2) + powf(ry - CAMPFIRE_Y, 2));

        if (dist_clearing < 20.0f || dist_lake < LAKE_RADIUS || dist_campfire < 15.0f) continue; 

        int i = chunk->num_objects;

        chunk->objects[i].position.x = rx;
        chunk->objects[i].position.y = ry;
        chunk->objects[i].position.z = get_terrain_height(rx, ry) - 0.3f; 

        chunk->objects[i].scale = 1.0f + ((float)rand() / RAND_MAX) * 0.8f;
        chunk->objects[i].model_index = rand() % 16; 

        chunk->objects[i].radius = 0.3f * chunk->objects[i].scale;
        
        chunk->num_objects++;
    }
}

void render_forest_chunk(const Chunk* chunk, const Scene* scene, const Camera* camera) {

    for (int i = 0; i < chunk->num_objects; i++) {

        float ox = chunk->objects[i].position.x;
        float oy = chunk->objects[i].position.y;

        float ddx  = ox - camera->position.x;
        float ddy  = oy - camera->position.y;
        float dist = sqrtf(ddx*ddx + ddy*ddy);

        // LOD check

        if (dist > LOD_SKIP_DIST) continue;
        if (!is_visible(camera, ox, oy, chunk->objects[i].radius + 2.0f)) continue;
        if (dist > LOD_FULL_DIST && chunk->objects[i].radius < 0.5f) continue;

        glPushMatrix();

        glTranslatef(ox, oy, chunk->objects[i].position.z);
        glScalef(chunk->objects[i].scale, chunk->objects[i].scale, chunk->objects[i].scale);
        
        float rotation_angle = (ox * oy) * 100.0f; 

        glRotatef(rotation_angle, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f); 

        if (chunk->objects[i].model_index >= 0) {
            draw_model(&scene->models[chunk->objects[i].model_index]);
        }

        glPopMatrix();
    }
}