#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>

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

struct Line
{
    struct Point start;
    struct Point end;
    struct Color clr;
};

int main(int argc, char *argv[])
{

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL hiba: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "SDL Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800, 600,
        0);

    if (!window)
    {
        printf("Ablak hiba: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        printf("Renderer hiba: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
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

    struct Line lines[100];
    int line_count = 0;

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
                        if (line_count < 100) {
                            rb.x = ev.button.x;
                            rb.y = ev.button.y;
                            printf("Vegpont: %d, %d\n", (int)rb.x, (int)rb.y);
                            struct Line newLine = {lb, rb, selected_color};
                            lines[line_count++] = newLine;
                        }
                        has_lb = false;
                    }
                }
                break;

            case SDL_KEYDOWN:
                if (ev.key.keysym.sym == SDLK_ESCAPE) running = 0;
                if (ev.key.keysym.sym == SDLK_SPACE) line_count = 0;
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);

        for (int i = 0; i < line_count; i++) {
            SDL_SetRenderDrawColor(renderer, lines[i].clr.r, lines[i].clr.g, lines[i].clr.b, 255);
            SDL_RenderDrawLine(renderer, (int)lines[i].start.x, (int)lines[i].start.y, 
                                         (int)lines[i].end.x, (int)lines[i].end.y);
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