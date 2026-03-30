#ifndef BOUNCING_BALL_H
#define BOUNCING_BALL_H

#include "utils.h"

typedef struct BouncingBall {
    vec3 position;       
    vec3 velocity;      
    float radius;       
    float gravity;       
    float restitution;   
} BouncingBall;

// Init ball

void init_bouncing_ball(BouncingBall* ball, float x, float y, float z);

// Updating the ball's movement

void update_bouncing_ball(BouncingBall* ball, double time);

// Render the ball

void render_bouncing_ball(const BouncingBall* ball);

#endif /* BOUNCING_BALL_H */