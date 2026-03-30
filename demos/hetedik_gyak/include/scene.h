#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "pendulum.h"

// #include "bouncing_ball.h"
// #include "animal.h"

#include <obj/model.h>

typedef struct Scene
{
    Material material;
    
    Pendulum pendulum;
    // BouncingBall ball;
    // Animal animal;
    
} Scene;

// Init scene

void init_scene(Scene* scene);

// Set the light of the scene

void set_lighting();

// Set the current material

void set_material(const Material* material);

// Update the scene

void update_scene(Scene* scene, double time);

// Render the scene objects

void render_scene(const Scene* scene);

// Draw the origin point of the coordinate system

void draw_origin();

#endif /* SCENE_H */