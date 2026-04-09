#ifndef SCENE_H
#define SCENE_H

#include "model.h"
#include "camera.h"

#define MAX_OBJECTS 750
#define MAX_MODELS 20

#define LAKE_CENTER_X 18.0f
#define LAKE_CENTER_Y 18.0f
#define LAKE_RADIUS   22.0f

#define LOD_FULL_DIST  60.0f
#define LOD_SKIP_DIST  120.0f

typedef struct SceneObject {
    vec3 position;
    float scale;
    float radius;
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

void render_scene(const Scene* scene, const Camera* camera);

#endif /* SCENE_H */