#ifndef SCENE_H
#define SCENE_H

#include "camera.h"

typedef struct Scene {
    double sphere_angle;
} Scene;

/**
 * Initialize the scene by loading models.
 */
void init_scene(Scene* scene);

/**
 * Update the scene.
 */
void update_scene(Scene* scene);

/**
 * Render the scene objects.
 */
void render_scene(const Scene* scene);

/**
 * Draw the origin of the world coordinate system.
 */
void draw_origin();

// Helper function for drawing a sphere with quads
// radius: the radius of the sphere
// slices: the vertical grid
// stacks: the horizontal grid

void draw_sphere(double radius, int slices, int stacks);

#endif /* SCENE_H */
