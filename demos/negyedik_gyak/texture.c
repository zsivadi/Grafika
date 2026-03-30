#include "texture.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

GLuint load_texture(char* filename)
{
    SDL_Surface* surface;
    GLuint texture_name;

    surface = IMG_Load(filename);

    glGenTextures(1, &texture_name);

    glBindTexture(GL_TEXTURE_2D, texture_name);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, (Pixel*)(surface->pixels));

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return texture_name;
}

GLuint generate_procedural_texture(int type) {

    const int SIZE = 256;
    Pixel image[SIZE][SIZE];


    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {

            // Sakktábla

            if (type == 1) {

                int tile_size = 32;
                bool is_black = ((x / tile_size) + (y / tile_size)) % 2 == 0;

                image[y][x][0] = is_black ? 0 : 255; // R
                image[y][x][1] = is_black ? 0 : 255; // G
                image[y][x][2] = is_black ? 0 : 255; // B
            }

            // Körlap

            else if (type == 2) {

                int cx = SIZE / 2;
                int cy = SIZE / 2;
                int r = 100;

                if ((x - cx)*(x - cx) + (y - cy)*(y - cy) < r*r) {
                    image[y][x][0] = 255; image[y][x][1] = 0; image[y][x][2] = 0; 
                } else {
                    image[y][x][0] = 255; image[y][x][1] = 255; image[y][x][2] = 255; 
                }
            }

            // Színátmenet

            else if (type == 3) {
                image[y][x][0] = (GLubyte)((x / (float)SIZE) * 255.0f); 
                image[y][x][1] = (GLubyte)((y / (float)SIZE) * 255.0f); 
                image[y][x][2] = 128;                                  
            }
        }
    }

    GLuint tex_id;

    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SIZE, SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    return tex_id;
}
