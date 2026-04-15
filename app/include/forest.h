#ifndef FOREST_H
#define FOREST_H

#include "scene.h"
#include "camera.h"

void init_forest(Scene* scene, const char* obj_files[]);

void render_forest(const Scene* scene, const Camera* camera);

#endif /* FOREST_H */