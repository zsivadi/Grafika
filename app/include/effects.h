#ifndef EFFECTS_H
#define EFFECTS_H

#include <GL/gl.h>

// Renders an animated, billboarded fire particle effect that always faces the camera

void draw_fire(double uptime, float cam_x, float cam_y, float fire_x, float fire_y);

// Renders rising, fading smoke particles above the fire, orienting them towards the camera

void draw_smoke(double uptime, float cam_x, float cam_y, float fire_x, float fire_y);

void render_clouds(double uptime, float cam_x, float cam_y, GLuint cloud_texture);

void draw_grass_patch(float x, float y, float z, float scale);

#endif /* EFFECTS_H */