#include "lake.h"

#include "scene.h"

#include <GL/gl.h>
#include <math.h>

void render_lake(double uptime) {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);

    float water_diffuse[]   = { 0.35f, 0.65f, 0.9f, 0.75f };  
    float water_specular[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    float water_shininess[] = { 30.0f };                 

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, water_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, water_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, water_shininess);

    float waterLevel = -1.2f; 
    int rings = 12;      
    int segments = 36;  

    for (int r = 0; r < rings; r++) {

        float rad1 = (r * LAKE_RADIUS) / rings;
        float rad2 = ((r + 1) * LAKE_RADIUS) / rings;

        float damping1 = (1.0f - (rad1 / LAKE_RADIUS)) * (1.0f - (rad1 / LAKE_RADIUS));
        float phase1 = rad1 * 1.0f - uptime * 1.5f;
        float ripple1 = damping1 * 0.15f * sin(phase1);
        
        float damping2 = (1.0f - (rad2 / LAKE_RADIUS)) * (1.0f - (rad2 / LAKE_RADIUS));
        float phase2 = rad2 * 1.0f - uptime * 1.5f;
        float ripple2 = damping2 * 0.15f * sin(phase2);
        
        float deriv1 = 0.0f, deriv2 = 0.0f;
        if (rad1 > 0.001f) deriv1 = damping1 * 0.15f * cos(phase1);
        if (rad2 > 0.001f) deriv2 = damping2 * 0.15f * cos(phase2);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= segments; i++) {

            float angle = (i * 2.0f * M_PI) / segments;
            float cos_a = cos(angle);
            float sin_a = sin(angle);

            float nx1 = -cos_a * deriv1;
            float ny1 = -sin_a * deriv1;
            float nz1 = 1.0f;
            float len1 = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1);

            glNormal3f(nx1/len1, ny1/len1, nz1/len1);
            glVertex3f(LAKE_CENTER_X + cos_a * rad1, LAKE_CENTER_Y + sin_a * rad1, waterLevel + ripple1);

            float nx2 = -cos_a * deriv2;
            float ny2 = -sin_a * deriv2;
            float nz2 = 1.0f;
            float len2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2);
            
            glNormal3f(nx2/len2, ny2/len2, nz2/len2);
            glVertex3f(LAKE_CENTER_X + cos_a * rad2, LAKE_CENTER_Y + sin_a * rad2, waterLevel + ripple2);
        }
        glEnd();
    }

    float default_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, default_specular);

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
}