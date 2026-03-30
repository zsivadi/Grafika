#ifndef BALL_H
#define BALL_H
#include <GL/gl.h>

/**
 * Ball position and speed
 */
typedef struct Ball
{
    float x;
    float y;
    float radius;
    float speed_x;
    float speed_y;
    float angle;
    GLuint texture_id;
} Ball;

/**
 * Move the ball to the given position.
 */
void init_ball(Ball* ball, float x, float y, float angle, GLuint texture_id);

/**
 * Update the ball.
 * @param time elapsed time in seconds.
 */
void update_ball(Ball* ball, double time);

// Increase/decrease the ball's radius with the given size

void resize_ball(Ball* ball, float size);

/**
 * Render the ball.
 */
void render_ball(Ball* ball);

#endif /* BALL_H */
