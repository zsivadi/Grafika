#include "pong.h"
#include <stdio.h>
#include <GL/gl.h>

#define LEFT_WALL 50
#define RIGHT_WALL (pong->width - 50)

void init_pong(Pong* pong, int width, int height, int left_score, int right_score)
{
    pong->width = width;
    pong->height = height;
    pong->left_score = left_score;
    pong->right_score = right_score;
    init_pad(&(pong->left_pad), 0, height, RED_THEME);
    init_pad(&(pong->right_pad), width - 50, height, GREEN_THEME);
    init_ball(&(pong->ball), width / 2, height / 2, 15.0);

}

void update_pong(Pong* pong, double time)
{
    update_pad(&(pong->left_pad), time);
    update_pad(&(pong->right_pad), time);
    update_ball(&(pong->ball), time);
    bounce_ball(pong);
    update_score(pong);
}

void render_pong(Pong* pong)
{

    render_score(pong);

    render_pad(&(pong->left_pad));
    render_pad(&(pong->right_pad));
    render_ball(&(pong->ball));
}

void render_score(Pong* pong)
{
    
    glColor3f(1.0, 1.0, 1.0);

    for(int i = 0; i < pong->left_score; i++) {
        glBegin(GL_QUADS);
        glVertex2f(50 + i*20, 10);
        glVertex2f(60 + i*20, 10);
        glVertex2f(60 + i*20, 30);
        glVertex2f(50 + i*20, 30);
        glEnd();
    }

    for(int i = 0; i < pong->right_score; i++) {
        glBegin(GL_QUADS);
        glVertex2f(pong->width - 60 - i*20, 10);
        glVertex2f(pong->width - 50 - i*20, 10);
        glVertex2f(pong->width - 50 - i*20, 30);
        glVertex2f(pong->width - 60 - i*20, 30);
        glEnd();
    }
}

void set_left_pad_position(Pong* pong, float position)
{
    set_pad_position(&(pong->left_pad), position);
}

void set_left_pad_speed(Pong* pong, float speed)
{
    set_pad_speed(&(pong->left_pad), speed);
}

void set_right_pad_position(Pong* pong, float position)
{
    set_pad_position(&(pong->right_pad), position);
}

void set_right_pad_speed(Pong* pong, float speed)
{
    set_pad_speed(&(pong->right_pad), speed);
}

void bounce_ball(Pong* pong)
{
    /*
    if (pong->ball.x - pong->ball.radius < LEFT_WALL) {
        pong->ball.x = pong->ball.radius + 50;
        pong->ball.speed_x *= -1;
    }
    if (pong->ball.x + pong->ball.radius > RIGHT_WALL) {
        pong->ball.x = pong->width - pong->ball.radius - 50;
        pong->ball.speed_x *= -1;
    }*/

    if (pong->ball.y - pong->ball.radius < 50) {
        pong->ball.y = pong->ball.radius + 50;
        pong->ball.speed_y *= -1;
        pong->ball.angle = -1;
        printf("Up\n");
    }
    if (pong->ball.y + pong->ball.radius > pong->height) {
        pong->ball.y = pong->height - pong->ball.radius;
        pong->ball.speed_y *= -1;
        pong->ball.angle = -1;
        printf("Down\n");
    }
    

    if ((pong->ball.x - pong->ball.radius <= pong->left_pad.x + pong->left_pad.width) && 
        ((pong->ball.y - pong->ball.radius <= pong->left_pad.y + pong->left_pad.height) &&
        (pong->ball.y + pong->ball.radius >= pong->left_pad.y))) {
        pong->ball.x = pong->ball.radius + 50;
        pong->ball.speed_x *= -1;
        pong->ball.angle = -1;
        printf("Left\n");
    }
    if ((pong->ball.x + pong->ball.radius >= pong->right_pad.x) &&
        ((pong->ball.y - pong->ball.radius <= pong->right_pad.y + pong->right_pad.height) &&
        (pong->ball.y + pong->ball.radius >= pong->right_pad.y))) {
        pong->ball.x = pong->width - pong->ball.radius - 50;
        pong->ball.speed_x *= -1;
        pong->ball.angle = -1;
        printf("Right\n");
    }
}

void update_score(Pong* pong) {

    if (pong->ball.x - pong->ball.radius < 0) {
        pong->right_score++;

        pong->ball.radius = 0;
        pong->ball.speed_x = 0;
        pong->ball.speed_y = 0;
        printf("Out left\n");

    } else if (pong->ball.x + pong->ball.radius > pong->width) {
        pong->left_score++;

        pong->ball.radius = 0;
        pong->ball.speed_x = 0;
        pong->ball.speed_y = 0;
        printf("Out right\n");

    }
}