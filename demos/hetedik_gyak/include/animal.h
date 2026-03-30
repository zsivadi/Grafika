#ifndef ANIMAL_H
#define ANIMAL_H

#include "utils.h"

typedef struct LegAngles {
    float hip_angle;    
    float knee_angle;  
} LegAngles;

typedef struct Animal {
    vec3 position;         
    float rotation;         
    double time_elapsed;    
    float walk_speed;       
    
    float body_length;
    float body_width;
    float body_height;
    
    float thigh_length;     
    float shin_length;      
    
    LegAngles leg_angles[4]; 
} Animal;

// Init animal

void init_animal(Animal* animal, float x, float y, float z);

// Update animal movement

void update_animal(Animal* animal, double time);

// Draw animal

void render_animal(const Animal* animal);

#endif /* ANIMAL_H */