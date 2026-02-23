#ifndef CIRCLE_H
#define CIRCLE_H

#include <SDL2/SDL.h>

/**
 * Data of a circle object in Descartes coordinate system
 */
typedef struct Color {
    int r;
    int g;
    int b;
} Color;

typedef struct Circle
{
	double x;
	double y;
	double radius;
    struct Color clr;

} Circle;

/**
 * Set the data of the circle
 */
void set_circle_data(Circle* circle, double x, double y, double radius, Color clr);

/**
 * Calculate the area of the circle.
 */
double calc_circle_area(const Circle* circle);

void draw_circle_segments(SDL_Renderer* renderer, const Circle* c, int steps);
int point_in_circle(int mx, int my, const Circle* c);

#endif // CIRCLE_H

