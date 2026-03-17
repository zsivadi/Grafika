#include "scene.h"

#include <obj/load.h>
#include <obj/draw.h>
#include <time.h>

void init_scene(Scene* scene)
{
    load_model(&(scene->cube), "assets/models/cube.obj");
    load_model(&(scene->cat), "assets/models/cat.obj");


    scene->texture_id = load_texture("assets/textures/cube.png");
    glBindTexture(GL_TEXTURE_2D, scene->texture_id);

    scene->model_x = 2.0f; 
    scene->model_y = 0.0f;

    scene->rotation = 0.0f;
    scene->model_x = 0.0f;
    scene->model_y = 0.0f;

    scene->material.ambient.red = 0.0;
    scene->material.ambient.green = 0.0;
    scene->material.ambient.blue = 0.0;

    scene->material.diffuse.red = 1.0;
    scene->material.diffuse.green = 1.0;
    scene->material.diffuse.blue = 0.0;

    scene->material.specular.red = 0.0;
    scene->material.specular.green = 0.0;
    scene->material.specular.blue = 0.0;

    scene->material.shininess = 0.0;
}

void set_lighting()
{
    float ambient_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float diffuse_light[] = { 1.0f, 1.0f, 1.0, 1.0f };
    float specular_light[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    float position[] = { 0.0f, 0.0f, 10.0f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular_light);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void set_material(const Material* material)
{
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

void update_scene(Scene* scene, double time)
{
    scene->rotation += 10.0f * (float)time;
}

void render_scene(const Scene* scene)
{
    set_material(&(scene->material));
    set_lighting();
    draw_origin();

    // 4x4-es grid feltöltése modellekkel 
    // Minden második modell forgatása

    /*
    float spacing = 2.0f;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            
            glPushMatrix();

                glTranslatef((i - 1.5f) * spacing, (j - 1.5f) * spacing, 0.0f);

                if ((i + j) % 2 == 0) {
                    glRotatef(scene->rotation, 0, 0, 1);
                    draw_model(&(scene->cube));
                } else {
                    draw_model(&(scene->cat));
                }

            glPopMatrix();

        } 
    }
    */

    glPushMatrix();
        glTranslatef(-2.0f, 0.0f, 0.0f);
        draw_model(&(scene->cube));
    glPopMatrix();

    glPushMatrix();
        glTranslatef(scene->model_x, scene->model_y, 0.0f);
        draw_model(&(scene->cat));
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
