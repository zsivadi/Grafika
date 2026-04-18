#ifndef FOREST_H
#define FOREST_H

#include "scene.h"
#include "camera.h"

// Initializes the global forest parameters and loads necessary model assets

void init_forest(Scene* scene, const char* obj_files[]);

// Populates a specific terrain chunk with randomly scaled and positioned trees, rocks, and bushes

void init_forest_chunk(Scene* scene, Chunk* chunk);

// Renders the visible objects within a chunk, applying Level of Detail (LOD) and frustum culling

void render_forest_chunk(const Chunk* chunk);

#endif /* FOREST_H */