#ifndef VISIBILITY_H
#define VISIBILITY_H

#include "camera.h"

int is_visible(const Camera* camera, float ox, float oy, float obj_radius);

#endif /* VISIBILITY_H */