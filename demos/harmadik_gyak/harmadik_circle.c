#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

#include "sdl_utils.h"
#include "circle.h"

#define MAX_CIRCLE_COUNT 100

int main(int argc, char *argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (init_sdl(&window, &renderer) != 0) {
        return 1;
    }

    Circle circles[MAX_CIRCLE_COUNT];
    int circle_count = 0;

    int running = 1;
    SDL_Event ev;

    int center_x = 0, center_y = 0;
    int has_center = 0;

    int selected = -1;

    while (running) {

        while (SDL_PollEvent(&ev)) {

            switch (ev.type) {

            case SDL_QUIT:
                running = 0;
                break;

            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE)
                    running = 0;
                break;

            case SDL_MOUSEBUTTONDOWN:

                if (ev.button.button == SDL_BUTTON_LEFT) {

                    for (int i = 0; i < circle_count; i++) {
                        if (point_in_circle(ev.button.x, ev.button.y, &circles[i])) {
                            selected = i;
                        }
                    }

                    if (selected == -1) {
                        center_x = ev.button.x;
                        center_y = ev.button.y;
                        has_center = 1;
                    }
                }

                else if (ev.button.button == SDL_BUTTON_RIGHT && has_center) {

                    int dx = ev.button.x - center_x;
                    int dy = ev.button.y - center_y;
                    double radius = sqrt(dx*dx + dy*dy);

                    Color clr = { rand()%256, rand()%256, rand()%256 };

                    if (circle_count < MAX_CIRCLE_COUNT) {
                        set_circle_data(&circles[circle_count++],
                                        center_x,
                                        center_y,
                                        radius,
                                        clr);
                    }

                    has_center = 0;
                }

                break;

            case SDL_MOUSEMOTION:
                if (selected != -1) {
                    circles[selected].x = ev.motion.x;
                    circles[selected].y = ev.motion.y;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                selected = -1;
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < circle_count; i++) {
            draw_circle_segments(renderer, &circles[i], 60);
        }

        int mx, my;
        SDL_GetMouseState(&mx, &my);

        for (int i = 0; i < circle_count; i++) {
            if (point_in_circle(mx, my, &circles[i])) {

                SDL_SetRenderDrawColor(renderer, 255,255,255,255);

                SDL_RenderDrawLine(renderer,
                    circles[i].x - 10, circles[i].y,
                    circles[i].x + 10, circles[i].y);

                SDL_RenderDrawLine(renderer,
                    circles[i].x,
                    circles[i].y - 10,
                    circles[i].x,
                    circles[i].y + 10);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}