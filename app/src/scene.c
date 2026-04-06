#include "scene.h"

#include <math.h>
#include <GL/gl.h>

void init_scene(Scene* scene) {
    scene->uptime = 0.0;
}

void update_scene(Scene* scene, double time) {
    scene->uptime += time;
}

void render_scene(const Scene* scene) {

    glEnable(GL_COLOR_MATERIAL);

    //glColor3f(0.0f, 0.9f, 0.4f);

    float size = 100.0f;
    float step = 2.5f; 
    for (float x = -size; x < size; x += step) {

        glBegin(GL_QUAD_STRIP);
        for (float y = -size; y <= size; y += step) {
            
            float dist = sqrt(x*x + y*y); 

            float angle = atan2(y, x);
            float distortion = sin(angle * 5.0f) * 2.0f + cos(angle * 3.0f) * 1.5f;
            float final_radius = 12.0f + distortion;
            
            if (dist < final_radius) { 
                glColor3f(0.45f, 0.4f, 0.25f); 
            } else if (dist < final_radius + 10.0f) {
                float t = (dist - final_radius) / 10.0f;
                glColor3f(0.45f*(1-t) + 0.05f*t, 0.4f*(1-t) + 0.2f*t, 0.25f*(1-t) + 0.05f*t);
            } else {
                float local_noise = (sin(x * 0.5f) * cos(y * 0.5f)) * 0.05f;
                glColor3f(0.05f, 0.15f + local_noise, 0.05f);
            }
            
            glNormal3f(0, 0, 1);
            
            glTexCoord2f(x/10.0f, y/10.0f); 
            glVertex3f(x, y, 0);

            glTexCoord2f((x+step)/10.0f, y/10.0f); 
            glVertex3f(x + step, y, 0);
        }
        glEnd();
    }


    glColor3f(0.8f, 0.2f, 0.2f);
    glBegin(GL_QUADS);
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glVertex3f( 1.0f, -1.0f, 0.0f);
        glVertex3f( 1.0f, -1.0f, 3.0f);
        glVertex3f(-1.0f, -1.0f, 3.0f);
        
        glVertex3f(-1.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f, 1.0f, 0.0f);
        glVertex3f( 1.0f, 1.0f, 3.0f);
        glVertex3f(-1.0f, 1.0f, 3.0f);
        
        glVertex3f(-1.0f, -1.0f, 0.0f);
        glVertex3f(-1.0f,  1.0f, 0.0f);
        glVertex3f(-1.0f,  1.0f, 3.0f);
        glVertex3f(-1.0f, -1.0f, 3.0f);
        
        glVertex3f(1.0f, -1.0f, 0.0f);
        glVertex3f(1.0f,  1.0f, 0.0f);
        glVertex3f(1.0f,  1.0f, 3.0f);
        glVertex3f(1.0f, -1.0f, 3.0f);
        
        glVertex3f(-1.0f, -1.0f, 3.0f);
        glVertex3f( 1.0f, -1.0f, 3.0f);
        glVertex3f( 1.0f,  1.0f, 3.0f);
        glVertex3f(-1.0f,  1.0f, 3.0f);
    glEnd();

    glDisable(GL_COLOR_MATERIAL);
}