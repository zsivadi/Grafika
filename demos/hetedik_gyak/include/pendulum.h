#ifndef PENDULUM_H
#define PENDULUM_H

#include "utils.h"

typedef struct Pendulum
{
    vec3 position;       
    float length;        
    float current_angle; 
    double time_elapsed; 
} Pendulum;

// Creating the pendulum at the a given position

void init_pendulum(Pendulum* pendulum, float x, float y, float z);

// Update the pendulum's position based on the elapsed time

void update_pendulum(Pendulum* pendulum, double time);

// Draw the pendulum

void render_pendulum(const Pendulum* pendulum);

#endif /* PENDULUM_H */