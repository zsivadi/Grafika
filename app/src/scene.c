#include "scene.h"

#include <math.h>
#include <GL/gl.h>

void init_scene(Scene* scene) {
    scene->uptime = 0.0;
}

void update_scene(Scene* scene, double time) {
    scene->uptime += time;
}

float smoothstep(float edge0, float edge1, float x) {

    float t = (x - edge0) / (edge1 - edge0);

    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return t * t * (3.0f - 2.0f * t);
}

void render_scene(const Scene* scene) {

    glEnable(GL_COLOR_MATERIAL);

    //glColor3f(0.0f, 0.9f, 0.4f);

    float size = 100.0f;
    float step = 2.0f; 

    float lakeCenterX = 18.0f;
    float lakeCenterY = 18.0f;
    float lakeRadius = 20.0f;
    float maxDepth = 5.0f;

    float clearingColor[3] = {0.45f, 0.4f,  0.25f};
    float forestColor[3]   = {0.02f, 0.12f, 0.02f}; 

    for (float x = -size; x < size; x += step) {

        glBegin(GL_QUAD_STRIP);
        for (float y = -size; y <= size; y += step) {
            
            float noise1 = sin(x * 0.15f + y * 0.1f) * 3.0f + cos(x * 0.3f - y * 0.2f) * 1.5f;
            float dist1 = sqrt(x*x + y*y) + noise1;
            float t1 = smoothstep(5.0f, 25.0f, dist1);

            float dist_lake1 = sqrt(pow(x - lakeCenterX, 2) + pow(y - lakeCenterY, 2));
            float z1 = 0.0f;

            if (dist_lake1 < lakeRadius) {

                float depth_factor = 1.0f - (dist_lake1 / lakeRadius); 
                z1 = -maxDepth * (depth_factor * depth_factor); 
            }
            
            float r1 = clearingColor[0] * (1.0f - t1) + forestColor[0] * t1;
            float g1 = clearingColor[1] * (1.0f - t1) + forestColor[1] * t1;
            float b1 = clearingColor[2] * (1.0f - t1) + forestColor[2] * t1;

            if (z1 < -0.1f) {
                r1 *= 0.6f; 
                g1 *= 0.6f; 
                b1 *= 0.8f; 
            } 

            glColor3f(r1, g1, b1);
            glNormal3f(0, 0, 1);
            glTexCoord2f(x/10.0f, y/10.0f); 
            glVertex3f(x, y, z1);
            
            float x2 = x + step;
            float noise2 = sin(x2 * 0.15f + y * 0.1f) * 3.0f + cos(x2 * 0.3f - y * 0.2f) * 1.5f;
            float dist2 = sqrt(x2*x2 + y*y) + noise2;
            float t2 = smoothstep(5.0f, 25.0f, dist2);

            float dist_lake2 = sqrt(pow(x2 - lakeCenterX, 2) + pow(y - lakeCenterY, 2));
            float z2 = 0.0f;

            if (dist_lake2 < lakeRadius) {

                float depth_factor = 1.0f - (dist_lake2 / lakeRadius);
                z2 = -maxDepth * (depth_factor * depth_factor);
            }

            float r2 = clearingColor[0] * (1.0f - t2) + forestColor[0] * t2;
            float g2 = clearingColor[1] * (1.0f - t2) + forestColor[1] * t2;
            float b2 = clearingColor[2] * (1.0f - t2) + forestColor[2] * t2;

           if (z2 < -0.1f) {
                r2 *= 0.6f; 
                g2 *= 0.6f; 
                b2 *= 0.8f;
            }

            glColor3f(r2, g2, b2);
            glTexCoord2f(x2/10.0f, y/10.0f); 
            glVertex3f(x2, y, z2);
        }
        glEnd();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.4f, 0.6f, 0.85f); 

    float waterLevel = -0.5f; 

    glBegin(GL_QUADS);
        glNormal3f(0, 0, 1);
        glVertex3f(lakeCenterX - lakeRadius, lakeCenterY - lakeRadius, waterLevel);
        glVertex3f(lakeCenterX + lakeRadius, lakeCenterY - lakeRadius, waterLevel);
        glVertex3f(lakeCenterX + lakeRadius, lakeCenterY + lakeRadius, waterLevel);
        glVertex3f(lakeCenterX - lakeRadius, lakeCenterY + lakeRadius, waterLevel);
    glEnd();

    glDisable(GL_BLEND);

    glDisable(GL_COLOR_MATERIAL);
}