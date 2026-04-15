#ifndef DUCKS_H
#define DUCKS_H

#include "scene.h"

void init_ducks(Duck ducks[2]);

void update_ducks(Duck ducks[2], double time);

void render_ducks(const Scene* scene);

#endif /* DUCKS_H */