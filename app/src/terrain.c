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

    float n1 = fractal_noise(x * 0.015f, y * 0.015f, 4, 0.45f);
    float n2 = fractal_noise(x * 0.06f, y * 0.06f, 3, 0.4f);
    
    float elevation = powf(n1, 3.0f) * 35.0f - 2.0f;
    elevation += n2 * 1.5f;

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

static void get_terrain_color(float x, float y, float height, float* r, float* g, float* b) {

    float dist_clearing = sqrtf(x * x + y * y);

    float dx_lake = x - LAKE_CENTER_X;
    float dy_lake = y - LAKE_CENTER_Y;
    float dist_lake = sqrtf(dx_lake * dx_lake + dy_lake * dy_lake);

    float clearingColor[3] = {0.45f, 0.4f, 0.25f};  
    float forestColor[3] = {0.095f, 0.50f, 0.01f};   
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

static void apply_rock_material_and_bump(float x, float y, float z, float* nx, float* ny, float* nz) {
    
    float rb = smoothstep(4.5f, 6.5f, z);

    float spec_val = rb * 0.4f; 
    float spec_color[] = {spec_val, spec_val, spec_val, 1.0f};
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec_color);
    glMaterialf(GL_FRONT, GL_SHININESS, 8.0f + rb * 56.0f); 

    if (rb > 0.0f) {
        float rough = (hash(x * 7.0f, y * 7.0f) - 0.5f) * 0.6f * rb;
        *nx += rough; 
        *ny += rough;
        
        float l = sqrtf((*nx)*(*nx) + (*ny)*(*ny) + (*nz)*(*nz));
        if (l > 0.001f) { *nx /= l; *ny /= l; *nz /= l; }
    }
}

GLuint init_terrain_chunk(int cx, int cy) {

    GLuint list_id = glGenLists(1);
    glNewList(list_id, GL_COMPILE);

    float step = 2.0f; 
    int grid_cells = (int)(CHUNK_SIZE / step);
    int cache_size = grid_cells + 2;
    
    float start_x = cx * CHUNK_SIZE;
    float start_y = cy * CHUNK_SIZE;

    float hmap[35][35];

    for (int i = 0; i < cache_size; i++) {
        for (int j = 0; j < cache_size; j++) {
            hmap[i][j] = get_terrain_height(start_x + i * step, start_y + j * step);
        }
    }

    glEnable(GL_COLOR_MATERIAL);
    glDisable(GL_ALPHA_TEST);

    glDisable(GL_TEXTURE_2D);

    for (int i = 0; i < grid_cells; i++) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= grid_cells; j++) {
            
            float x1 = start_x + i * step;
            float y1 = start_y + j * step;
            float z1 = hmap[i][j]; 
            
            float tx1 = step, ty1 = 0.0f, tz1 = hmap[i + 1][j] - z1;
            float ux1 = 0.0f, uy1 = step, uz1 = hmap[i][j + 1] - z1;
            
            float nx1 = ty1 * uz1 - tz1 * uy1;
            float ny1 = tz1 * ux1 - tx1 * uz1;
            float nz1 = tx1 * uy1 - ty1 * ux1;
            
            float len1 = sqrtf(nx1*nx1 + ny1*ny1 + nz1*nz1);
            if (len1 > 0.001f) { nx1 /= len1; ny1 /= len1; nz1 /= len1; }

            float r1, g1, b1;
            get_terrain_color(x1, y1, z1, &r1, &g1, &b1);
            apply_rock_material_and_bump(x1, y1, z1, &nx1, &ny1, &nz1);
            
            glColor3f(r1, g1, b1);
            glNormal3f(nx1, ny1, nz1);
            glVertex3f(x1, y1, z1);
            
            float x2 = start_x + (i + 1) * step;
            float y2 = start_y + j * step;
            float z2 = hmap[i + 1][j];

            float tx2 = step, ty2 = 0.0f, tz2 = hmap[i + 2][j] - z2;
            float ux2 = 0.0f, uy2 = step, uz2 = hmap[i + 1][j + 1] - z2;

            float nx2 = ty2 * uz2 - tz2 * uy2;
            float ny2 = tz2 * ux2 - tx2 * uz2;
            float nz2 = tx2 * uy2 - ty2 * ux2;

            float len2 = sqrtf(nx2*nx2 + ny2*ny2 + nz2*nz2);
            if (len2 > 0.001f) { nx2 /= len2; ny2 /= len2; nz2 /= len2; }

            float r2, g2, b2;
            get_terrain_color(x2, y2, z2, &r2, &g2, &b2);
            apply_rock_material_and_bump(x2, y2, z2, &nx2, &ny2, &nz2);
            
            glColor3f(r2, g2, b2);
            glNormal3f(nx2, ny2, nz2);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_EQUAL); 

    for (int i = 0; i < grid_cells; i++) {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= grid_cells; j++) {
            
            float x1 = start_x + i * step;
            float y1 = start_y + j * step;
            float z1 = hmap[i][j]; 
            
            float tx1 = step, ty1 = 0.0f, tz1 = hmap[i + 1][j] - z1;
            float ux1 = 0.0f, uy1 = step, uz1 = hmap[i][j + 1] - z1;
            
            float nx1 = ty1 * uz1 - tz1 * uy1;
            float ny1 = tz1 * ux1 - tx1 * uz1;
            float nz1 = tx1 * uy1 - ty1 * ux1;
            
            float len1 = sqrtf(nx1*nx1 + ny1*ny1 + nz1*nz1);
            if (len1 > 0.001f) { nx1 /= len1; ny1 /= len1; nz1 /= len1; }

            float r1, g1, b1;
            get_terrain_color(x1, y1, z1, &r1, &g1, &b1);
            
            float alpha1 = 1.0f;
            if (z1 > 7.0f) alpha1 = 1.0f - smoothstep(7.0f, 9.5f, z1);

            glColor4f(r1, g1, b1, alpha1);
            glNormal3f(nx1, ny1, nz1);
            glTexCoord2f(x1 * 0.25f, y1 * 0.25f);
            glVertex3f(x1, y1, z1);
            
            float x2 = start_x + (i + 1) * step;
            float y2 = start_y + j * step;
            float z2 = hmap[i + 1][j];

            float tx2 = step, ty2 = 0.0f, tz2 = hmap[i + 2][j] - z2;
            float ux2 = 0.0f, uy2 = step, uz2 = hmap[i + 1][j + 1] - z2;

            float nx2 = ty2 * uz2 - tz2 * uy2;
            float ny2 = tz2 * ux2 - tx2 * uz2;
            float nz2 = tx2 * uy2 - ty2 * ux2;

            float len2 = sqrtf(nx2*nx2 + ny2*ny2 + nz2*nz2);
            if (len2 > 0.001f) { nx2 /= len2; ny2 /= len2; nz2 /= len2; }

            float r2, g2, b2;
            get_terrain_color(x2, y2, z2, &r2, &g2, &b2);
            
            float alpha2 = 1.0f;
            if (z2 > 7.0f) alpha2 = 1.0f - smoothstep(7.0f, 9.5f, z2);

            glColor4f(r2, g2, b2, alpha2);
            glNormal3f(nx2, ny2, nz2);
            glTexCoord2f(x2 * 0.25f, y2 * 0.25f);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }

    glDepthFunc(GL_LESS);
    glDisable(GL_BLEND);

    glEndList(); 
    return list_id;
}