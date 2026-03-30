#include "pendulum.h"

#include <GL/gl.h>
#include <math.h>

void draw_sphere(float radius, int slices, int stacks) {

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

void init_pendulum(Pendulum* pendulum, float x, float y, float z) 
{
    pendulum->position.x = x;
    pendulum->position.y = y;
    pendulum->position.z = z;

    pendulum->length = 3.0f; 
    pendulum->current_angle = 0.0f;
    pendulum->time_elapsed = 0.0;
}

void update_pendulum(Pendulum* pendulum, double time) {
    
    pendulum->time_elapsed += time;

    pendulum->current_angle = 40.0f * sin(pendulum->time_elapsed * M_PI / 3.0);
}

void render_pendulum(const Pendulum* pendulum) {

    glPushMatrix();

    glTranslatef(pendulum->position.x, pendulum->position.y, pendulum->position.z);

    glRotatef(pendulum->current_angle, 1.0f, 0.0f, 0.0f);

    glDisable(GL_LIGHTING); 
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.8f, 0.8f, 0.8f);

    glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, -pendulum->length); 
    glEnd();

    glEnable(GL_LIGHTING);

    glTranslatef(0.0f, 0.0f, -pendulum->length);

    glColor3f(1.0f, 0.2f, 0.2f);
    draw_sphere(0.5f, 20, 20); 

    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
}