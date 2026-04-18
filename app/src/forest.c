#include "forest.h"
#include "terrain.h"
#include "lake.h"

#include <GL/gl.h>
#include <math.h>

#include <stdlib.h>

void init_forest_chunk(Scene* scene, Chunk* chunk) {

    chunk->num_objects = 0; 
    float start_x = chunk->cx * CHUNK_SIZE;
    float start_y = chunk->cy * CHUNK_SIZE;

    int attempts = 0;

    while (chunk->num_objects < MAX_CHUNK_OBJECTS && attempts < 500) {

        attempts++;

        float rx = start_x + ((float)rand() / RAND_MAX) * CHUNK_SIZE;
        float ry = start_y + ((float)rand() / RAND_MAX) * CHUNK_SIZE;

        float rz = get_terrain_height(rx, ry);
        if (rz > 8.0f) continue;

        float dist_clearing = sqrtf(rx*rx + ry*ry);
        float dist_lake     = sqrtf(powf(rx - LAKE_CENTER_X, 2) + powf(ry - LAKE_CENTER_Y, 2));
        float dist_campfire = sqrtf(powf(rx - CAMPFIRE_X, 2) + powf(ry - CAMPFIRE_Y, 2));

        if (dist_clearing < 20.0f || dist_lake < LAKE_RADIUS || dist_campfire < 15.0f) continue; 

        int i = chunk->num_objects;

        chunk->objects[i].position.x = rx;
        chunk->objects[i].position.y = ry;
        chunk->objects[i].position.z = rz - 0.3f; 

        chunk->objects[i].scale = 1.0f + ((float)rand() / RAND_MAX) * 0.8f;
        chunk->objects[i].model_index = rand() % 16; 

        chunk->objects[i].radius = 0.3f * chunk->objects[i].scale;
        
        chunk->num_objects++;
    }

    chunk->forest_display_list = glGenLists(1);
    glNewList(chunk->forest_display_list, GL_COMPILE);

    for (int j = 0; j < chunk->num_objects; j++) {

        glPushMatrix();

        glTranslatef(chunk->objects[j].position.x, chunk->objects[j].position.y, chunk->objects[j].position.z);
        glScalef(chunk->objects[j].scale, chunk->objects[j].scale, chunk->objects[j].scale);
        
        float rotation_angle = (chunk->objects[j].position.x * chunk->objects[j].position.y) * 100.0f; 

        glRotatef(rotation_angle, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

        if (chunk->objects[j].model_index >= 0) {
            draw_model(&scene->models[chunk->objects[j].model_index]);
        }

        glPopMatrix();
    }
    
    glEndList();
}

void render_forest_chunk(const Chunk* chunk) {
   glCallList(chunk->forest_display_list);
}