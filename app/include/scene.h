#ifndef SCENE_H
#define SCENE_H

#include "model.h"

#define MAX_OBJECTS 600
#define MAX_MODELS 20

typedef struct SceneObject {
    vec3 position;
    float scale;
    int model_index; 
} SceneObject;

typedef struct Scene {
    double uptime;
    SceneObject objects[MAX_OBJECTS]; 
    Model models[MAX_MODELS];
    int num_loaded_models;    
    int num_objects;
} Scene;


void init_scene(Scene* scene);

void update_scene(Scene* scene, double time);

void render_scene(const Scene* scene);

#endif /* SCENE_H */