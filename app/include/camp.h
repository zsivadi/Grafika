#ifndef CAMP_H
#define CAMP_H

#include "scene.h"

// Initializes the campsite and setting up tent positions 

void init_camp(Scene* scene);

// Renders the campsite, including the tents, campfire model, and dynamic light source

void render_camp(const Scene* scene);

#endif /* CAMP_H */