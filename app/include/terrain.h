#ifndef TERRAIN_H
#define TERRAIN_H

#include "scene.h"
#include <GL/gl.h>

// Terrain generation parameters

#define TERRAIN_MAX_HEIGHT 3.0f

// Smoothstep interpolation function

float smoothstep(float edge0, float edge1, float x);

// Get terrain height at given world coordinates (x, y)

float get_terrain_height(float x, float y);

// Render the terrain mesh with elevation and colors

GLuint init_terrain_chunk(int cx, int cy);

#endif /* TERRAIN_H */