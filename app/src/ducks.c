#include "ducks.h"

#include <GL/gl.h>

#include <math.h>
#include <stdlib.h>

static void duck_random_dir(Duck* duck) {

    float dx = duck->x - LAKE_CENTER_X;
    float dy = duck->y - LAKE_CENTER_Y;
    float dist = sqrtf(dx*dx + dy*dy);

    float angle = ((float)(rand() % 1000) / 1000.0f) * 2.0f * (float)M_PI;

    float nx = cosf(angle);
    float ny = sinf(angle);

    if (dist > LAKE_RADIUS * 0.6f) {

        float inward_x = -dx / dist;
        float inward_y = -dy / dist;

        float blend = ((float)(rand() % 1000) / 1000.0f - 0.5f);

        nx = inward_x + blend * inward_y;
        ny = inward_y - blend * inward_x;
    }

    float len = sqrtf(nx*nx + ny*ny);

    if (len < 0.001f) len = 1.0f;

    duck->dir_x = nx / len;
    duck->dir_y = ny / len;
}

void init_ducks(Duck ducks[2]) {

    for (int d = 0; d < 2; d++) {

        float start_angle = (float)d * (float)M_PI;

        ducks[d].x = LAKE_CENTER_X + cosf(start_angle) * (LAKE_RADIUS * 0.5f);
        ducks[d].y = LAKE_CENTER_Y + sinf(start_angle) * (LAKE_RADIUS * 0.5f);

        duck_random_dir(&ducks[d]);
    }
}

void update_ducks(Duck ducks[2], double time) {

    float duck_speed = 1.2f;

    for (int d = 0; d < 2; d++) {

        Duck* duck = &ducks[d];

        duck->x += duck->dir_x * duck_speed * (float)time;
        duck->y += duck->dir_y * duck_speed * (float)time;

        float dx = duck->x - LAKE_CENTER_X;
        float dy = duck->y - LAKE_CENTER_Y;

        float dist = sqrtf(dx*dx + dy*dy);

        if (dist >= LAKE_RADIUS * 0.55f) {

            float nd = LAKE_RADIUS * 0.55f;

            duck->x = LAKE_CENTER_X + (dx / dist) * nd;
            duck->y = LAKE_CENTER_Y + (dy / dist) * nd;

            duck_random_dir(duck);
        }
    }
}

void render_ducks(const Scene* scene) {

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    float waterLevel = -1.35f;

    for (int d = 0; d < 2; d++) {

        const Duck* duck = &scene->ducks[d];

        float ddx = duck->x - LAKE_CENTER_X;
        float ddy = duck->y - LAKE_CENTER_Y;

        if (sqrtf(ddx*ddx + ddy*ddy) > LAKE_RADIUS * 0.75f) continue;

        glPushMatrix();
        glTranslatef(duck->x, duck->y, waterLevel - 0.35f);

        float heading = atan2f(duck->dir_y, duck->dir_x);

        glRotatef(heading * 180.0f / (float)M_PI + 90.0f, 0.0f, 0.0f, 1.0f);
        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);

        glScalef(0.5f, 0.5f, 0.5f);

        draw_model(&scene->duck_model);
        
        glPopMatrix();
    }
}