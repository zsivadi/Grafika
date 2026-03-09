#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include "sdl_utils.h"

struct Point
{
    double x;
    double y;
};

struct Color
{
    int r;
    int g;
    int b;
};

struct Rect
{
    struct Point start;
    int width;
    int height;
    struct Color clr;
};

int main(int argc, char *argv[])
{

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (init_sdl(&window, &renderer) != 0) {
        return 1;
    }

    int running = 1;
    SDL_Event ev;

    struct Point lb;
    struct Point rb;
    bool has_lb = false;

    struct Color palette[] = {
        {255, 0, 0},
        {0, 255, 0},
        {0, 0, 255},
        {255, 255, 0},
        {255, 0, 255},
        {0, 255, 255},
        {255, 255, 255}
    };
    int palette_size = 7;
    struct Color selected_color = {255, 255, 255};

    struct Rect rects[100];
    int rect_count = 0;

    while (running)
    {
        while (SDL_PollEvent(&ev))
        {
            switch (ev.type)
            {
            case SDL_QUIT:
                running = 0;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (ev.button.y < 50 && ev.button.button == SDL_BUTTON_LEFT) {
                    int index = (ev.button.x - 5) / 45;
                    if (index >= 0 && index < palette_size) {
                        selected_color = palette[index];
                    }
                } else {
                    if (ev.button.button == SDL_BUTTON_LEFT) {
                        lb.x = ev.button.x;
                        lb.y = ev.button.y;
                        has_lb = true;
                        printf("Kezdopont: %d, %d\n", (int)lb.x, (int)lb.y);
                    } else if (ev.button.button == SDL_BUTTON_RIGHT && has_lb) {
                        if (rect_count < 100) {
                            rb.x = ev.button.x;
                            rb.y = ev.button.y;
                            printf("Vegpont: %d, %d\n", (int)rb.x, (int)rb.y);
                            struct Rect newRect = {lb, rb.x - lb.x, rb.y - lb.y, selected_color};
                            rects[rect_count++] = newRect;
                        }
                        has_lb = false;
                    }
                }
                break;

            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE) running = 0;
                if (ev.key.keysym.sym == SDLK_SPACE) rect_count = 0;
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);

        for (int i = 0; i < rect_count; i++) {
            SDL_SetRenderDrawColor(renderer, rects[i].clr.r, rects[i].clr.g, rects[i].clr.b, 255);
            SDL_Rect box = {rects[i].start.x, rects[i].start.y, rects[i].width, rects[i].height};
            SDL_RenderFillRect(renderer, &box);
        }
        
        for (int i = 0; i < palette_size; i++) {
            SDL_Rect box = { i * 45 + 5, 5, 40, 40 };
            SDL_SetRenderDrawColor(renderer, palette[i].r, palette[i].g, palette[i].b, 255);
            SDL_RenderFillRect(renderer, &box);

            if (palette[i].r == selected_color.r && palette[i].g == selected_color.g && palette[i].b == selected_color.b) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                SDL_RenderDrawRect(renderer, &box);
            }
        }
        
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}