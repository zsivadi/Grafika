#include "scene.h"

#include "ducks.h"
#include "terrain.h"
#include "lake.h"
#include "effects.h"
#include "camp.h"
#include "interactions.h"
#include "forest.h"

#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>

#include <stdio.h>

static void free_chunk(Chunk* chunk) {

    if (chunk->is_active) {

        if (chunk->terrain_display_list != 0) {
            glDeleteLists(chunk->terrain_display_list, 1);
        }

        chunk->is_active = false;
        chunk->terrain_display_list = 0;
    }
}

static void load_chunk(Chunk* chunk, int cx, int cy) {

    chunk->cx = cx;
    chunk->cy = cy;
    chunk->is_active = true;

    unsigned int seed = (unsigned int)(cx * 73856093 ^ cy * 19349663);
    srand(seed);

    chunk->terrain_display_list = init_terrain_chunk(cx, cy);

    chunk->num_objects = 0;
    init_forest_chunk(chunk);
}

void init_scene(Scene* scene) {

    scene->uptime = 60.0;
    scene->num_loaded_models = 0;
    scene->fire_lit = 1;
    scene->fire_brightness = 1.0f;

    const char* obj_files[] = {

        "assets/models/BirchTree_1.obj", "assets/models/BirchTree_1.obj",
        "assets/models/BirchTree_2.obj", "assets/models/BirchTree_3.obj",
        "assets/models/BirchTree_3.obj", "assets/models/BirchTree_4.obj",
        "assets/models/BirchTree_5.obj", "assets/models/BirchTree_5.obj",
        "assets/models/DeadTree_1.obj",  "assets/models/DeadTree_2.obj",
        "assets/models/Rock_1.obj",      "assets/models/Rock_2.obj",
        "assets/models/Rock_3.obj",      "assets/models/Bush.obj",
        "assets/models/Bush.obj",        "assets/models/Bush.obj"
    };
    int num_files_to_load = sizeof(obj_files) / sizeof(obj_files[0]);

    for (int i = 0; i < num_files_to_load && i < MAX_MODELS; i++) {
        if (load_model(&scene->models[scene->num_loaded_models], obj_files[i])) {
            scene->num_loaded_models++;
        }
    }

    load_model(&scene->campfire_model, "assets/models/Campfire.obj");
    load_model(&scene->tent_model,     "assets/models/Tent.obj");
    load_model(&scene->duck_model,     "assets/models/MallardDuck.obj");

    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++) {

        scene->active_chunks[i].is_active = false;
        scene->active_chunks[i].terrain_display_list = 0;
    }

    // Fix models now only generate in the 0,0 chunk

    init_camp(scene);
    init_ducks(scene->ducks);
}

void update_scene(Scene* scene, const Camera* camera, double time) {

    scene->uptime += time;
    update_ducks(scene->ducks, time);

    int player_cx = (int)floor(camera->position.x / CHUNK_SIZE);
    int player_cy = (int)floor(camera->position.y / CHUNK_SIZE);

    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++) {
        if (scene->active_chunks[i].is_active) {

            int dx = abs(scene->active_chunks[i].cx - player_cx);
            int dy = abs(scene->active_chunks[i].cy - player_cy);

            if (dx > RENDER_DISTANCE || dy > RENDER_DISTANCE) {
                free_chunk(&scene->active_chunks[i]);
            }
        }
    }

    bool chunk_loaded_this_frame = false;

    for (int y = player_cy - RENDER_DISTANCE; y <= player_cy + RENDER_DISTANCE; y++) {
        for (int x = player_cx - RENDER_DISTANCE; x <= player_cx + RENDER_DISTANCE; x++) {
            
            bool already_loaded = false;

            for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++) {

                if (scene->active_chunks[i].is_active && 

                    scene->active_chunks[i].cx == x && 
                    scene->active_chunks[i].cy == y) {
                    already_loaded = true;

                    break;
                }
            }

            if (!already_loaded) {
                for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++) {

                    if (!scene->active_chunks[i].is_active) {

                        load_chunk(&scene->active_chunks[i], x, y);
                        chunk_loaded_this_frame = true;
                        break;
                    }
                }
            }

            if (chunk_loaded_this_frame) break;
        }

        if (chunk_loaded_this_frame) break;
    }
}

void render_scene(const Scene* scene, const Camera* camera) {

    for (int i = 0; i < MAX_ACTIVE_CHUNKS; i++) {

        if (scene->active_chunks[i].is_active) {

            glCallList(scene->active_chunks[i].terrain_display_list);
            
            render_forest_chunk(&scene->active_chunks[i], scene, camera);
        }
    }

    render_camp(scene);
    render_ducks(scene);
    render_lake(scene->uptime);

    if (scene->fire_lit) {

        glPushMatrix();

        float fire_z = get_terrain_height(CAMPFIRE_X, CAMPFIRE_Y);
        glTranslatef(CAMPFIRE_X, CAMPFIRE_Y, fire_z + 0.1f);
        
        draw_fire(scene->uptime, camera->position.x, camera->position.y, CAMPFIRE_X, CAMPFIRE_Y);
        draw_smoke(scene->uptime, camera->position.x, camera->position.y, CAMPFIRE_X, CAMPFIRE_Y);

        glPopMatrix();
    }
}