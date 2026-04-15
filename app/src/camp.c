#include "camp.h"

#include <GL/gl.h>

#include <math.h>

void init_camp(Scene* scene) {

    scene->campfire_radius = 0.5f;
    scene->tent_radius     = 1.0f;  

    float tent_dist = 5.0f;

    for (int i = 0; i < 3; i++) {

        float a = (float)i * (2.0f * (float)M_PI / 3.0f);

        scene->tent_positions[i].x = CAMPFIRE_X + cosf(a) * tent_dist;
        scene->tent_positions[i].y = CAMPFIRE_Y + sinf(a) * tent_dist;
        scene->tent_positions[i].z = 0.0f;
    }
}

void render_camp(const Scene* scene) {

    glEnable(GL_TEXTURE_2D);
    
    // Tents

    for (int i = 0; i < 3; i++) {

        glPushMatrix();
        glTranslatef(scene->tent_positions[i].x, scene->tent_positions[i].y, 0.35f);

        float angle_to_fire = atan2f(CAMPFIRE_Y - scene->tent_positions[i].y,
                                     CAMPFIRE_X - scene->tent_positions[i].x);

        glRotatef(angle_to_fire * 180.0f / (float)M_PI + 90.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
        glScalef(3.0f, 3.0f, 1.5f);

        draw_model(&scene->tent_model);

        glPopMatrix();
    }

    // Campfire

    glPushMatrix();

    glTranslatef(CAMPFIRE_X, CAMPFIRE_Y, 0.2f);

    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    draw_model(&scene->campfire_model);

    glPopMatrix();

    // Light

    if (scene->fire_lit) {

        float flicker      = (float)(sin(scene->uptime * 13.7) * 0.15 + 0.85);
        float brightness   = scene->fire_brightness * flicker;

        float fire_diffuse[] = { 1.0f * brightness, 0.45f * brightness, 0.05f * brightness, 1.0f };
        float fire_pos[]     = { CAMPFIRE_X, CAMPFIRE_Y, 0.8f, 1.0f };

        glEnable(GL_LIGHT1);

        glLightfv(GL_LIGHT1, GL_DIFFUSE,  fire_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, fire_diffuse);
        glLightfv(GL_LIGHT1, GL_POSITION, fire_pos);

        glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION,  1.0f);
        glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION,    0.18f);
        glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.08f);
        
    } else {
        glDisable(GL_LIGHT1);
    }
}