#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/gl.h>

typedef GLubyte Pixel[3];

/**
 * Load texture from file and returns with the texture name.
 */
GLuint load_texture(char* filename);

// Function for pre-coded procedural texture generating options

GLuint generate_procedural_texture(int type);

#endif /* TEXTURE_H */
