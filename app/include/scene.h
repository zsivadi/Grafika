#ifndef SCENE_H
#define SCENE_H

#include "model.h"
#include "camera.h"

// Maximum number of scene objects and model types

#define MAX_OBJECTS 750
#define MAX_MODELS 25

// Lake position and size parameters

#define LAKE_CENTER_X 18.0f
#define LAKE_CENTER_Y 18.0f
#define LAKE_RADIUS   22.0f

// Level of Detail distance thresholds

#define LOD_FULL_DIST  60.0f
#define LOD_SKIP_DIST  120.0f

// Campfire position in world space

#define CAMPFIRE_X  -8.0f
#define CAMPFIRE_Y  -8.0f

typedef struct Duck {
    float x, y;
    float dir_x, dir_y;
} Duck;

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

    Model campfire_model;
    Model tent_model;
    Model duck_model;

    int fire_lit;
    Duck ducks[2];

    vec3 tent_positions[3];
    float tent_radius;
    float campfire_radius;
    float fire_brightness;
} Scene;

// Initialize the scene with all objects, models, and game state

void init_scene(Scene* scene);

// Update scene state including animations, duck movement, and time progression

void update_scene(Scene* scene, double time);

// Render all scene geometry including terrain, objects, water, fire effects

void render_scene(const Scene* scene, const Camera* camera);

// Handle player interactions with campfire (toggle) and tents (sleep/skip time)
// Returns: 0 = no interaction, 1 = campfire toggled, 2 = slept in tent

int scene_interact(Scene* scene, const Camera* camera, double* uptime);

#endif /* SCENE_H */