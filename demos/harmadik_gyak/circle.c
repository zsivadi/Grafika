#include "circle.h"

#include <math.h>

void set_circle_data(Circle* circle, double x, double y, double radius, Color clr)
{
	circle->x = x;
	circle->y = y;
	circle->radius = (radius > 0.0) ? radius : NAN;
    circle->clr = clr;
}

double calc_circle_area(const Circle* circle)
{
	double area = circle->radius * circle->radius * M_PI;
	return area;
}

void draw_circle_segments(SDL_Renderer* renderer, const Circle* c, int steps)
{
    SDL_SetRenderDrawColor(renderer, c->clr.r, c->clr.g, c->clr.b, 255);

    double step = 2 * M_PI / steps;

    for (int i = 0; i < steps; i++) {
        double t1 = i * step;
        double t2 = (i + 1) * step;

        int x1 = c->x + c->radius * cos(t1);
        int y1 = c->y + c->radius * sin(t1);

        int x2 = c->x + c->radius * cos(t2);
        int y2 = c->y + c->radius * sin(t2);

        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
}

int point_in_circle(int mx, int my, const Circle* c)
{
    double dx = mx - c->x;
    double dy = my - c->y;
    return (dx*dx + dy*dy) <= (c->radius * c->radius);
}