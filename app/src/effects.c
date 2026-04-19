#include "settings.h"

#include "effects.h"

#include <GL/gl.h>

#include <math.h>

void draw_fire(double uptime, float cam_x, float cam_y, float fire_x, float fire_y) {

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);

    int num_particles = 18;

    for (int i = 0; i < num_particles; i++) {

        float phase   = (float)i / num_particles;
        float t       = (float)fmod(uptime * 1.8 + phase, 1.0);
        float flicker = (float)(sin(uptime * 12.0 + i * 2.3) * 0.1 + 0.9);

        float px = sinf(phase * 6.28f + (float)uptime * 0.7f) * 0.25f * (1.0f - t);
        float py = cosf(phase * 4.71f + (float)uptime * 0.5f) * 0.25f * (1.0f - t);
        float pz = t * 1.8f;

        float size  = (0.35f - t * 0.25f) * flicker;
        float alpha = (1.0f - t) * 0.45f * flicker;  

        float r = 1.0f; float g = 0.4f + (1.0f - t) * 0.4f; float b = t * 0.05f;

        float to_cam_x = cam_x - fire_x;
        float to_cam_y = cam_y - fire_y;

        float tc_len = sqrtf(to_cam_x*to_cam_x + to_cam_y*to_cam_y);

        if (tc_len < 0.001f) tc_len = 1.0f;

        float right_x = -to_cam_y / tc_len;
        float right_y =  to_cam_x / tc_len;

        glColor4f(r, g, b, alpha);

        glBegin(GL_QUADS);
            glVertex3f(px + right_x * (-size),       py + right_y * (-size),       pz);
            glVertex3f(px + right_x * ( size),       py + right_y * ( size),       pz);
            glVertex3f(px + right_x * ( size*0.5f),  py + right_y * ( size*0.5f),  pz + size * 2.0f);
            glVertex3f(px + right_x * (-size*0.5f),  py + right_y * (-size*0.5f),  pz + size * 2.0f);
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

void draw_smoke(double uptime, float cam_x, float cam_y, float fire_x, float fire_y) {

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    int num_puffs = 8;

    for (int i = 0; i < num_puffs; i++) {
        float phase = (float)i / num_puffs;
        float t     = (float)fmod(uptime * 0.4 + phase, 1.0);

        float px = sinf(phase * 5.1f + (float)uptime * 0.2f) * 0.15f * t;
        float py = cosf(phase * 3.7f + (float)uptime * 0.15f) * 0.15f * t;
        float pz = 1.8f + t * 3.5f;

        float size  = 0.2f + t * 0.8f;
        float alpha = (1.0f - t) * 0.12f;  
        float grey  = 0.5f + t * 0.2f;

        float to_cam_x = cam_x - fire_x;
        float to_cam_y = cam_y - fire_y;
        float tc_len = sqrtf(to_cam_x*to_cam_x + to_cam_y*to_cam_y);

        if (tc_len < 0.001f) tc_len = 1.0f;

        float rx = -to_cam_y / tc_len;
        float ry =  to_cam_x / tc_len;

        glColor4f(grey, grey, grey, alpha);

        glBegin(GL_QUADS);
            glVertex3f(px + rx*(-size), py + ry*(-size), pz);
            glVertex3f(px + rx*( size), py + ry*( size), pz);
            glVertex3f(px + rx*( size), py + ry*( size), pz + size*1.5f);
            glVertex3f(px + rx*(-size), py + ry*(-size), pz + size*1.5f);
        glEnd();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
}

void render_clouds(double uptime, float cam_x, float cam_y, GLuint cloud_texture) {

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cloud_texture);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
    glDepthMask(GL_FALSE); 

    float base_height = 60.0f; 
    float cell_size = 30.0f;  
    int grid_radius = 6;       
    float scroll_speed = 1.5f; 

    float virtual_x = cam_x - (float)uptime * scroll_speed;
    float virtual_y = cam_y; 

    int start_cx = (int)floorf(virtual_x / cell_size) - grid_radius;
    int start_cy = (int)floorf(virtual_y / cell_size) - grid_radius;

    glBegin(GL_QUADS);

    for (int x = 0; x < grid_radius * 2; x++) {
        for (int y = 0; y < grid_radius * 2; y++) {
            
            int cx = start_cx + x;
            int cy = start_cy + y;

            unsigned int seed = (unsigned int)(WORLD_SEED ^ (cx * 73856093) ^ (cy * 19349663));
            
            #define RND() (seed = (seed * 1103515245 + 12345) & 0x7fffffff, (float)seed / 0x7fffffff)

            int num_clouds = (int)(RND() * 3.99f); 

            for (int i = 0; i < num_clouds; i++) {
                
                float offset_x = RND() * cell_size;
                float offset_y = RND() * cell_size;
                
                float scale_x = 15.0f + RND() * 25.0f; 
                float scale_y = 15.0f + RND() * 25.0f; 
                
                float height = base_height + RND() * 8.0f;

                float base_alpha = 0.2f + RND() * 0.6f;

                float wx = cx * cell_size + offset_x + (float)uptime * scroll_speed;
                float wy = cy * cell_size + offset_y;

                float dx = wx - cam_x;
                float dy = wy - cam_y;
                float dist = sqrtf(dx*dx + dy*dy);
                float max_dist = grid_radius * cell_size;

                if (dist > max_dist) continue; 

                float fade = 1.0f - (dist / max_dist);
                fade = fade * fade; 

                glColor4f(1.0f, 1.0f, 1.0f, base_alpha * fade);

                glTexCoord2f(0.0f, 0.0f); glVertex3f(wx - scale_x, wy - scale_y, height);
                glTexCoord2f(1.0f, 0.0f); glVertex3f(wx + scale_x, wy - scale_y, height);
                glTexCoord2f(1.0f, 1.0f); glVertex3f(wx + scale_x, wy + scale_y, height);
                glTexCoord2f(0.0f, 1.0f); glVertex3f(wx - scale_x, wy + scale_y, height);
            }
        }
    }
    glEnd();

    #undef RND

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

void draw_grass_patch(float x, float y, float z, float scale) {

    float w = 0.8f * scale; 
    float h = 0.5f * scale; 

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_QUADS);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(x - w, y, z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x + w, y, z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x + w, y, z + h);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x - w, y, z + h);

        glTexCoord2f(0.0f, 1.0f); glVertex3f(x, y - w, z);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(x, y + w, z);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(x, y + w, z + h);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(x, y - w, z + h);

    glEnd();
    
}