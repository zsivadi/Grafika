#include "animal.h"

#include <GL/gl.h>
#include <math.h>
#include <stdlib.h> 

void draw_volumetric_box(float length, float width, float height) {

    float x1 = -width / 2.0f;
    float x2 = width / 2.0f;

    float y1 = -height / 2.0f;
    float y2 = height / 2.0f;

    float z1 = -length;
    float z2 = 0.0f;

    glBegin(GL_QUADS);
        glNormal3f(-1.0f, 0.0f, 0.0f);
        glVertex3f(x1, y2, z1); glVertex3f(x1, y1, z1); glVertex3f(x1, y1, z2); glVertex3f(x1, y2, z2);

        glNormal3f(1.0f, 0.0f, 0.0f);
        glVertex3f(x2, y1, z1); glVertex3f(x2, y2, z1); glVertex3f(x2, y2, z2); glVertex3f(x2, y1, z2);
    
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(x1, y1, z1); glVertex3f(x2, y1, z1); glVertex3f(x2, y2, z1); glVertex3f(x1, y2, z1);
        
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x2, y1, z2); glVertex3f(x1, y1, z2); glVertex3f(x1, y2, z2); glVertex3f(x2, y2, z2);
        
        glNormal3f(0.0f, -1.0f, 0.0f);
        glVertex3f(x1, y1, z1); glVertex3f(x2, y1, z1); glVertex3f(x2, y1, z2); glVertex3f(x1, y1, z2);
        
        glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x2, y2, z1); glVertex3f(x1, y2, z1); glVertex3f(x1, y2, z2); glVertex3f(x2, y2, z2);
    glEnd();
}

void init_animal(Animal* animal, float x, float y, float z) {

    animal->position.x = x;
    animal->position.y = y;
    animal->position.z = z;

    animal->rotation = 0.0f;
    animal->time_elapsed = 0.0;
    animal->walk_speed = 1.0f;
    
    animal->body_length = 2.0f;
    animal->body_width = 1.0f;
    animal->body_height = 0.8f;
    
    animal->thigh_length = 1.0f;
    animal->shin_length = 0.8f;
    
    for (int i = 0; i < 4; i++) {

        animal->leg_angles[i].hip_angle = 0.0f;
        animal->leg_angles[i].knee_angle = 0.0f;
    }
}

void update_animal(Animal* animal, double time) {

    animal->time_elapsed += time;
    
    float distance = animal->walk_speed * time;
    animal->position.x += distance;
    
    if (animal->position.x > 8.0f) {
        animal->position.x = -8.0f;
    }

    float cycle_time = 1.5f;
    float phase = (float)(fmod(animal->time_elapsed, cycle_time) / cycle_time * 2.0 * M_PI);
    
    float phases[4] = {
        phase,
        phase + (float)M_PI,
        phase + (float)M_PI,
        phase
    };
    
    for (int i = 0; i < 4; i++) {
        animal->leg_angles[i].hip_angle = 25.0f * sin(phases[i]);
        
        if (animal->leg_angles[i].hip_angle > 0) {
            animal->leg_angles[i].knee_angle = 40.0f * fabs(animal->leg_angles[i].hip_angle / 25.0f);
        } else {
            animal->leg_angles[i].knee_angle = 0.0f;
        }
    }
}

void render_animal(const Animal* animal) {

    glPushMatrix();

    glTranslatef(animal->position.x, animal->position.y, animal->position.z);
    
    glRotatef(animal->rotation, 0.0f, 0.0f, 1.0f);

    glDisable(GL_TEXTURE_2D);
    glColor3f(0.7f, 0.5f, 0.2f); 

    glPushMatrix();
        glTranslatef(0.0f, 0.0f, animal->body_height / 2.0f); 
        draw_volumetric_box(animal->body_height, animal->body_length, animal->body_width);
    glPopMatrix();
    
    float lx = animal->body_length / 2.0f; 
    float ly = animal->body_width / 2.0f;  
    float lz = 0.0f; 

    float leg_positions[4][3] = {
        {lx, ly, lz},   
        {lx, -ly, lz},  
        {-lx, ly, lz},  
        {-lx, -ly, lz}   
    };

    glColor3f(0.5f, 0.35f, 0.05f); 

    for (int i = 0; i < 4; i++) {

        glPushMatrix();
        
            glTranslatef(leg_positions[i][0], leg_positions[i][1], leg_positions[i][2]);
            
            glRotatef(animal->leg_angles[i].hip_angle, 0.0f, 1.0f, 0.0f);
            
            draw_volumetric_box(animal->thigh_length, 0.2f, 0.2f);
            
            glTranslatef(0.0f, 0.0f, -animal->thigh_length);
            
            glRotatef(animal->leg_angles[i].knee_angle, 0.0f, 1.0f, 0.0f);
            
            draw_volumetric_box(animal->shin_length, 0.15f, 0.15f);
        
        glPopMatrix();
    }
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
}