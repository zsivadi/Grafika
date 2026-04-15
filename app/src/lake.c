#include "lake.h"

#include "scene.h"

#include <GL/gl.h>
#include <math.h>

void render_lake(double uptime) {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_COLOR_MATERIAL);

    float water_diffuse[]   = { 0.1f, 0.4f, 0.7f, 0.85f }; 
    float water_specular[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    float water_shininess[] = { 20.0f };                 

    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, water_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, water_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, water_shininess);

    float waterLevel = -0.5f; 
    float waterStep  = 0.5f;

    for (float wx = LAKE_CENTER_X - LAKE_RADIUS; wx < LAKE_CENTER_X + LAKE_RADIUS; wx += waterStep) {

        glBegin(GL_QUAD_STRIP);

        for (float wy = LAKE_CENTER_Y - LAKE_RADIUS; wy <= LAKE_CENTER_Y + LAKE_RADIUS; wy += waterStep) {

            float dx1 = wx - LAKE_CENTER_X;
            float dy1 = wy - LAKE_CENTER_Y;

            float d1  = sqrt(dx1 * dx1 + dy1 * dy1);
            float ripple1 = 0.0f;
            float nx1 = 0.0f, ny1 = 0.0f, nz1 = 1.0f;
            
            if (d1 < LAKE_RADIUS) {

                float damping1 = pow(1.0f - (d1 / LAKE_RADIUS), 2.0f); 
                float phase1 = d1 * 1.0f - uptime * 1.5f;

                ripple1 = damping1 * 0.15f * sin(phase1);

                if (d1 > 0.001f) {

                    float derivative = damping1 * 0.15f * cos(phase1);

                    nx1 = -(dx1 / d1) * derivative;
                    ny1 = -(dy1 / d1) * derivative;

                    nz1 = 1.0f;
                    
                    float len = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1);
                    nx1 /= len; ny1 /= len; nz1 /= len;
                }
            }

            glNormal3f(nx1, ny1, nz1);
            glVertex3f(wx, wy, waterLevel + ripple1);

            float x2  = wx + waterStep;
            float dx2 = x2 - LAKE_CENTER_X;
            float dy2 = wy - LAKE_CENTER_Y;

            float d2  = sqrt(dx2*dx2 + dy2*dy2);

            float ripple2 = 0.0f;
            float nx2 = 0.0f, ny2 = 0.0f, nz2 = 1.0f;
            
            if (d2 < LAKE_RADIUS) {

                float damping2 = pow(1.0f - (d2 / LAKE_RADIUS), 2.0f);
                float phase2 = d2 * 1.0f - uptime * 1.5f;

                ripple2 = damping2 * 0.15f * sin(phase2);

                if (d2 > 0.001f) {

                    float derivative2 = damping2 * 0.15f * cos(phase2);

                    nx2 = -(dx2 / d2) * derivative2;
                    ny2 = -(dy2 / d2) * derivative2;

                    nz2 = 1.0f;
                    
                    float len2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2);
                    nx2 /= len2; ny2 /= len2; nz2 /= len2;
                }
            }

            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, wy, waterLevel + ripple2);

        }

        glEnd();
    }

    float default_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    glMaterialfv(GL_FRONT, GL_SPECULAR, default_specular);

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_BLEND);
}