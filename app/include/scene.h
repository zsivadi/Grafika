#ifndef SCENE_H
#define SCENE_H

typedef struct Scene {
    double uptime;
} Scene;


void init_scene(Scene* scene);

void update_scene(Scene* scene, double time);

void render_scene(const Scene* scene);

#endif /* SCENE_H */