#include "scene.h"

#include <GL/gl.h>

void init_scene(Scene* scene) {
    
    scene->material.ambient.red = 0.2f;
    scene->material.ambient.green = 0.2f;
    scene->material.ambient.blue = 0.2f;

    scene->material.diffuse.red = 0.8f;
    scene->material.diffuse.green = 0.8f;
    scene->material.diffuse.blue = 0.8f;

    scene->material.specular.red = 0.0f;
    scene->material.specular.green = 0.0f;
    scene->material.specular.blue = 0.0f;

    scene->material.shininess = 0.0f;

    init_pendulum(&(scene->pendulum), 0.0f, 0.0f, 4.0f);
}

void set_lighting() {

    float ambient_light[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    float diffuse_light[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    float specular_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    float position[] = { 5.0f, 10.0f, 5.0f, 1.0f }; 

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material) {

    float ambient_material_color[] = {
        material->ambient.red,
        material->ambient.green,
        material->ambient.blue
    };

    float diffuse_material_color[] = {
        material->diffuse.red,
        material->diffuse.green,
        material->diffuse.blue
    };

    float specular_material_color[] = {
        material->specular.red,
        material->specular.green,
        material->specular.blue
    };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular_material_color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, &(material->shininess));
}

void update_scene(Scene* scene, double time) {
    update_pendulum(&(scene->pendulum), time);
}

void render_scene(const Scene* scene) {

    set_lighting();
    set_material(&(scene->material));
    
    draw_origin();

    render_pendulum(&(scene->pendulum));
}

void draw_origin() {

    glDisable(GL_LIGHTING); 
    
    glBegin(GL_LINES);

    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(2.0f, 0.0f, 0.0f);

    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 2.0f, 0.0f);

    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 2.0f);

    glEnd();
    
    glEnable(GL_LIGHTING); 
}