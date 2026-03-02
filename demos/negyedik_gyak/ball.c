#include "ball.h"

#include <GL/gl.h>

#include <math.h>

void init_ball(Ball* ball, float x, float y, float angle)
{
    ball->x = x;
    ball->y = y;
    ball->radius = 50;
    ball->speed_x = 500;
    ball->speed_y = 500;
    ball->angle = angle;
}

void update_ball(Ball* ball, double time)
{
    ball->x += ball->speed_x * time;
    ball->y += ball->speed_y * time;

    float speed = sqrt(ball->speed_x * ball->speed_x + ball->speed_y * ball->speed_y);

    ball->angle += speed * 1.0f * time;
}

void resize_ball(Ball* ball, float size) {

    float new_radius = ball->radius + size;

    if (new_radius >= 15 && new_radius <= 85) {
        ball->radius = new_radius;
    }
}

void render_ball(Ball* ball)
{
    double angle;
    double x;
    double y;

    glPushMatrix();
    glTranslatef(ball->x, ball->y, 0.0);
    glRotatef(ball->angle, 0.0, 0.0, 1.0);

    glBegin(GL_TRIANGLE_FAN);
    glColor3f(1.0, 0.9, 0.8);
    glVertex2f(0, 0);

    angle = 0;
    while (angle < 2.0 * M_PI + 1) {
        x = cos(angle) * ball->radius;
        y = sin(angle) * ball->radius;
        glVertex2f(x, y);
        angle += 0.8;
    }
    glEnd();
    glPopMatrix();
}
