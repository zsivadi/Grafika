#include "terrain.h"
#include <GL/gl.h>
#include <math.h>

float smoothstep(float edge0, float edge1, float x) {

    float t = (x - edge0) / (edge1 - edge0);

    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return t * t * (3.0f - 2.0f * t);
}

static float hash(float x, float y) {

    float n = sin(x * 12.9898f + y * 78.233f) * 43758.5453f;
    return n - floor(n);
}

static float noise(float x, float y) {

    float ix = floor(x);
    float iy = floor(y);

    float fx = x - ix;
    float fy = y - iy;

    float u = fx * fx * (3.0f - 2.0f * fx);
    float v = fy * fy * (3.0f - 2.0f * fy);

    float a = hash(ix, iy);
    float b = hash(ix + 1.0f, iy);
    float c = hash(ix, iy + 1.0f);
    float d = hash(ix + 1.0f, iy + 1.0f);

    return a * (1.0f - u) * (1.0f - v) + b * u * (1.0f - v) + c * (1.0f - u) * v + d * u * v;
}

static float fractal_noise(float x, float y, int octaves, float persistence) {

    float total = 0.0f;

    float frequency = 1.0f;
    float amplitude = 1.0f;
    float max_value = 0.0f;

    for (int i = 0; i < octaves; i++) {

        total += noise(x * frequency, y * frequency) * amplitude;

        max_value += amplitude;
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / max_value;
}

float get_terrain_height(float x, float y) {

    float elevation = fractal_noise(x * 0.08f, y * 0.08f, 4, 0.5f);

    elevation += fractal_noise(x * 0.03f, y * 0.03f, 2, 0.6f) * 0.5f;
    elevation = (elevation - 0.5f) * TERRAIN_MAX_HEIGHT * 2.0f;

    float dist_clearing = sqrtf(x * x + y * y);

    if (dist_clearing < 50.0f) {

        float dx_lake = x - LAKE_CENTER_X;
        float dy_lake = y - LAKE_CENTER_Y;
        float dist_lake = sqrtf(dx_lake * dx_lake + dy_lake * dy_lake);

        float dx_camp = x - CAMPFIRE_X;
        float dy_camp = y - CAMPFIRE_Y;
        float dist_campfire = sqrtf(dx_camp * dx_camp + dy_camp * dy_camp);

        if (dist_clearing < 25.0f) {

            float clearing_factor = smoothstep(20.0f, 5.0f, dist_clearing);
            elevation = elevation * (1.0f - clearing_factor);
        }
        if (dist_campfire < 10.0f) {

            float campfire_factor = smoothstep(10.0f, 6.0f, dist_campfire);
            elevation = elevation * (1.0f - campfire_factor);
        }
        if (dist_lake < LAKE_RADIUS + 6.0f) {

            if (dist_lake < LAKE_RADIUS) {

                float depth_factor = 1.0f - (dist_lake / LAKE_RADIUS);
                elevation = -1.0f - 4.0f * (depth_factor * depth_factor);
            } else {

                float shore_blend = smoothstep(LAKE_RADIUS + 6.0f, LAKE_RADIUS, dist_lake);
                elevation = elevation * (1.0f - shore_blend) + (-1.0f) * shore_blend;
            }
        }
    }
    return elevation;
}

static void get_terrain_normal(float x, float y, float* nx, float* ny, float* nz) {

    float delta = 0.5f;

    float h_center = get_terrain_height(x, y);
    float h_right = get_terrain_height(x + delta, y);
    float h_up = get_terrain_height(x, y + delta);

    float tx = delta; float ty = 0.0f; float tz = h_right - h_center;
    float ux = 0.0f; float uy = delta; float uz = h_up - h_center;

    *nx = ty * uz - tz * uy;
    *ny = tz * ux - tx * uz;
    *nz = tx * uy - ty * ux;

    float len = sqrtf((*nx) * (*nx) + (*ny) * (*ny) + (*nz) * (*nz));

    if (len > 0.001f) { *nx /= len; *ny /= len; *nz /= len; }
}

static void get_terrain_color(float x, float y, float height, float* r, float* g, float* b) {

    float dist_clearing = sqrtf(x * x + y * y);

    float dx_lake = x - LAKE_CENTER_X;
    float dy_lake = y - LAKE_CENTER_Y;
    float dist_lake = sqrtf(dx_lake * dx_lake + dy_lake * dy_lake);

    float clearingColor[3] = {0.45f, 0.4f, 0.25f};  
    float forestColor[3] = {0.02f, 0.12f, 0.02f};   
    float hillColor[3] = {0.25f, 0.22f, 0.15f};     
    float rockColor[3] = {0.35f, 0.35f, 0.37f};     

    float clearing_to_forest = smoothstep(5.0f, 20.0f, dist_clearing);

    *r = clearingColor[0] * (1.0f - clearing_to_forest) + forestColor[0] * clearing_to_forest;
    *g = clearingColor[1] * (1.0f - clearing_to_forest) + forestColor[1] * clearing_to_forest;
    *b = clearingColor[2] * (1.0f - clearing_to_forest) + forestColor[2] * clearing_to_forest;

    if (height > 1.5f) {

        float hill_blend = smoothstep(1.5f, 3.5f, height);

        *r = *r * (1.0f - hill_blend) + hillColor[0] * hill_blend;
        *g = *g * (1.0f - hill_blend) + hillColor[1] * hill_blend;
        *b = *b * (1.0f - hill_blend) + hillColor[2] * hill_blend;
    }
    if (height > 4.5f) {

        float rock_blend = smoothstep(4.5f, 6.5f, height);

        *r = *r * (1.0f - rock_blend) + rockColor[0] * rock_blend;
        *g = *g * (1.0f - rock_blend) + rockColor[1] * rock_blend;
        *b = *b * (1.0f - rock_blend) + rockColor[2] * rock_blend;
    }
    if (dist_lake < LAKE_RADIUS || height < -0.5f) {
        *r *= 0.5f; *g *= 0.5f; *b *= 0.65f;
    }
}

GLuint init_terrain_chunk(int cx, int cy) {

    GLuint list_id = glGenLists(1);
    glNewList(list_id, GL_COMPILE);

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_ALPHA_TEST);

    float step = 2.0f; 
    
    float start_x = cx * CHUNK_SIZE;
    float start_y = cy * CHUNK_SIZE;

    float end_x = start_x + CHUNK_SIZE;
    float end_y = start_y + CHUNK_SIZE;

    for (float x = start_x; x < end_x; x += step) {

        glBegin(GL_QUAD_STRIP);

        for (float y = start_y; y <= end_y; y += step) {

            float z1 = get_terrain_height(x, y);

            float nx1, ny1, nz1;
            get_terrain_normal(x, y, &nx1, &ny1, &nz1);

            float r1, g1, b1;
            get_terrain_color(x, y, z1, &r1, &g1, &b1);
            
            glColor3f(r1, g1, b1);
            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x, y, z1);
            
            float x2 = x + step;
            float z2 = get_terrain_height(x2, y);

            float nx2, ny2, nz2;
            get_terrain_normal(x2, y, &nx2, &ny2, &nz2);

            float r2, g2, b2;
            get_terrain_color(x2, y, z2, &r2, &g2, &b2);
            
            glColor3f(r2, g2, b2);
            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y, z2);
        }

        glEnd();
    }
    
    glEndList(); 
    
    return list_id;
}