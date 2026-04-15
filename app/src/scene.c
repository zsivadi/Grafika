#include "scene.h"

#include "ducks.h"
#include "terrain.h"
#include "lake.h"
#include "effects.h"
#include "camp.h"
#include "interactions.h"
#include "forest.h"

#include <stdio.h>
#include <GL/gl.h>

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

    init_camp(scene);
    init_ducks(scene->ducks);
    init_forest(scene, obj_files);
}

void update_scene(Scene* scene, double time) {

    scene->uptime += time;
    update_ducks(scene->ducks, time);
}

void render_scene(const Scene* scene, const Camera* camera) {

    render_terrain();

    render_lake(scene->uptime);
    render_camp(scene);
    render_ducks(scene);
    render_forest(scene, camera);

    if (scene->fire_lit) {

        glPushMatrix();
        glTranslatef(CAMPFIRE_X, CAMPFIRE_Y, 0.0f);

        draw_fire(scene->uptime, camera->position.x, camera->position.y, CAMPFIRE_X, CAMPFIRE_Y);
        draw_smoke(scene->uptime, camera->position.x, camera->position.y, CAMPFIRE_X, CAMPFIRE_Y);
        
        glPopMatrix();
    }
    
    glDisable(GL_COLOR_MATERIAL);
}