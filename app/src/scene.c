#include "scene.h"
#include <GL/gl.h>

void init_scene(Scene* scene) {
    scene->uptime = 0.0;
}

void update_scene(Scene* scene, double time) {
    scene->uptime += time;
}

void render_scene(const Scene* scene) {

    glEnable(GL_COLOR_MATERIAL);

    glColor3f(0.15f, 0.5f, 0.15f);

    glBegin(GL_QUADS);
        glVertex3f(-100.0f, -100.0f, 0.0f);
        glVertex3f( 100.0f, -100.0f, 0.0f);
        glVertex3f( 100.0f,  100.0f, 0.0f);
        glVertex3f(-100.0f,  100.0f, 0.0f);
    glEnd();

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