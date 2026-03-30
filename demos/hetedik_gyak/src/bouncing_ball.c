#include "bouncing_ball.h"

#include <GL/gl.h>
#include <math.h>

void draw_bouncing_sphere(float radius, int slices, int stacks) {

    for (int i = 0; i < stacks; i++) {

        float lat0 = M_PI * (-0.5f + (float)i / stacks);
        float z0  = sin(lat0);
        float zr0 = cos(lat0);

        float lat1 = M_PI * (-0.5f + (float)(i + 1) / stacks);
        float z1 = sin(lat1);
        float zr1 = cos(lat1);

        glBegin(GL_QUAD_STRIP);

        for (int j = 0; j <= slices; j++) {

            float lng = 2 * M_PI * (float)j / slices;
            float x = cos(lng);
            float y = sin(lng);

            glNormal3f(x * zr1, y * zr1, z1);
            glVertex3f(radius * x * zr1, radius * y * zr1, radius * z1);
            glNormal3f(x * zr0, y * zr0, z0);
            glVertex3f(radius * x * zr0, radius * y * zr0, radius * z0);
        }

        glEnd();
    }
}

void init_bouncing_ball(BouncingBall* ball, float x, float y, float z) {

    ball->position.x = x;
    ball->position.y = y;
    ball->position.z = z;
    
    ball->velocity.x = 2.0f; 
    ball->velocity.y = 1.0f; 
    ball->velocity.z = 0.0f; 
    
    ball->radius = 0.5f;
    ball->gravity = -9.81f;
    
    ball->restitution = 0.85f;
}

void update_bouncing_ball(BouncingBall* ball, double time) { 

    ball->velocity.z += ball->gravity * time;

    ball->position.z += ball->velocity.z * time;

    if (ball->position.z - ball->radius < 0.0f) {
        
        ball->position.z = ball->radius;
        
        ball->velocity.z = -ball->velocity.z * ball->restitution;
    }
}

void render_bouncing_ball(const BouncingBall* ball) {

    glPushMatrix();
    glTranslatef(ball->position.x, ball->position.y, ball->position.z);
    
    glColor3f(0.2f, 0.6f, 1.0f);
    draw_bouncing_sphere(ball->radius, 20, 20);
    
    glPopMatrix();
}