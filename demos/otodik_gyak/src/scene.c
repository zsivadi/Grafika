#include "scene.h"

#include <GL/gl.h>
#include <math.h>

void init_scene(Scene* scene) {

    scene->sphere_angle = 0.0;
}

void update_scene(Scene* scene) {

    scene->sphere_angle += 0.05;

    if (scene->sphere_angle > 360.0) {
        scene->sphere_angle -= 360.0;
    }
}

void render_scene(const Scene* scene) {
    draw_origin();
    /*
    glBegin(GL_QUADS);

        glColor3f(1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 1);
        glVertex3f(1, 0, 1);
        glVertex3f(1, 0, 0);

    glEnd();
    */

    // Triangle

    glBegin(GL_TRIANGLES);

        glColor3f(1, 1, 0); 
        glVertex3f(0, 0, 0);

        glColor3f(0, 1, 1); 
        glVertex3f(1, 0, 0);

        glColor3f(1, 0, 1); 
        glVertex3f(0.5, 0, 0.5);

    glEnd();

    // Sphere

    glColor3f(0.5, 0.5, 0.5);
    glPushMatrix();

        glTranslatef(0.5, 0.5, 0.5);

        glRotatef(scene->sphere_angle, 0, 0, 1.0);

        draw_sphere(0.5, 4, 4);

    glPopMatrix();
}

void draw_origin()
{
    glBegin(GL_LINES);

    glColor3f(1, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(1, 0, 0);

    glColor3f(0, 1, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 1, 0);

    glColor3f(0, 0, 1);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, 1);

    glEnd();
}

void draw_sphere(double radius, int slices, int stacks) {
    for (int i = 0; i < slices; i++) {
        for (int j = 0; j < stacks; j++) {
            double theta1 = (double)i * 2.0 * M_PI / slices;
            double theta2 = (double)(i + 1) * 2.0 * M_PI / slices;
            double phi1 = (double)j * M_PI / stacks;
            double phi2 = (double)(j + 1) * M_PI / stacks;

            glBegin(GL_QUADS);
            glVertex3f(radius * sin(phi1) * cos(theta1), radius * sin(phi1) * sin(theta1), radius * cos(phi1));
            glVertex3f(radius * sin(phi1) * cos(theta2), radius * sin(phi1) * sin(theta2), radius * cos(phi1));
            glVertex3f(radius * sin(phi2) * cos(theta2), radius * sin(phi2) * sin(theta2), radius * cos(phi2));
            glVertex3f(radius * sin(phi2) * cos(theta1), radius * sin(phi2) * sin(theta1), radius * cos(phi2));
            glEnd();
        }
    }
}
