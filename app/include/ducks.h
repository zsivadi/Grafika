#ifndef DUCKS_H
#define DUCKS_H

#include "scene.h"

// Initializes the ducks with starting positions and assigns them random movement directions

void init_ducks(Duck ducks[2]);

// Updates the position of the ducks over time and keeps them within the lake boundaries

void update_ducks(Duck ducks[2], double time);

// Renders the ducks on the surface of the lake, aligned with their movement direction

void render_ducks(const Scene* scene);

#endif /* DUCKS_H */