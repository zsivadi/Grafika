#ifndef INTERACTIONS_H
#define INTERACTIONS_H

#include "scene.h"
#include "camera.h"

// Checks for and executes player interactions with world objects based on camera view and distance

int scene_interact(Scene* scene, const Camera* camera, double* uptime);

#endif /* INTERACTIONS_H */