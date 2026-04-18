#ifndef VISIBILITY_H
#define VISIBILITY_H

#include "camera.h"

// Determines if an object is within the camera's field of view (frustum culling) to optimize rendering

int is_visible(const Camera* camera, float ox, float oy, float obj_radius);

#endif /* VISIBILITY_H */