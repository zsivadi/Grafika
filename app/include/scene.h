#ifndef SCENE_H
#define SCENE_H

#include "model.h"
#include "camera.h"

// Terrain and rendering parameters

#define CHUNK_SIZE 60.0f
#define RENDER_DISTANCE 1 
#define MAX_ACTIVE_CHUNKS ((RENDER_DISTANCE * 2 + 1) * (RENDER_DISTANCE * 2 + 1))
#define MAX_CHUNK_OBJECTS 150 

// Global limits

#define MAX_MODELS 25

// Lake geometry boundaries

#define LAKE_CENTER_X 18.0f
#define LAKE_CENTER_Y 18.0f
#define LAKE_RADIUS   22.0f

// Level of Detail (LOD) distances

#define LOD_FULL_DIST  60.0f
#define LOD_SKIP_DIST  120.0f

// Campfire coordinates

#define CAMPFIRE_X  -8.0f
#define CAMPFIRE_Y  -8.0f

// Structure representing a moving duck

typedef struct Duck {
    float x, y;
    float dir_x, dir_y;
} Duck;

// Structure for static environment objects 

typedef struct SceneObject {
    vec3 position;
    float scale;
    float radius;
    int model_index; 
} SceneObject;

// Structure representing a dynamically loaded segment of the world

typedef struct Chunk {
    int cx; 
    int cy; 
    bool is_active;
    
    GLuint terrain_display_list; 
    
    SceneObject objects[MAX_CHUNK_OBJECTS]; 
    int num_objects;
} Chunk;

// Main scene container holding the world state, models, and entities

typedef struct Scene {
    double uptime;

    Chunk active_chunks[MAX_ACTIVE_CHUNKS]; 
    
    Model models[MAX_MODELS];
    int num_loaded_models;    

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

// Initializes the core scene components, models, and static entities

void init_scene(Scene* scene);

// Updates dynamic elements (ducks, time) and manages chunk loading/unloading

void update_scene(Scene* scene, const Camera* camera, double time); 

// Renders all active chunks, entities, effects, and the lake

void render_scene(const Scene* scene, const Camera* camera);

// Handles player interactions (e.g., toggling fire, sleeping in tents)

int scene_interact(Scene* scene, const Camera* camera, double* uptime);

#endif /* SCENE_H */