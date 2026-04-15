#include "app.h"
#include "font8x8.h"

#include <GL/gl.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <math.h>

void init_app(App* app, int width, int height) {
    
    int error_code;
    int inited_loaders;

    app->is_running = false;
    app->uptime = 0.0;

    error_code = SDL_Init(SDL_INIT_EVERYTHING);
    if (error_code != 0) {
        printf("[ERROR] SDL initialization error: %s\n", SDL_GetError());
        return;
    }

    app->window = SDL_CreateWindow(
        "Camping Project",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);
        
    if (app->window == NULL) {

        printf("[ERROR] Unable to create the application window!\n");
        return;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

    inited_loaders = IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    if (inited_loaders == 0) {

        printf("[ERROR] IMG initialization error: %s\n", IMG_GetError());
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL) {

        printf("[ERROR] Unable to create the OpenGL context: %s\n", SDL_GetError());
        return;
    }

    app->uptime = (double)SDL_GetTicks() / 1000.0;

    init_opengl();

    int actual_width, actual_height;
    SDL_GetWindowSize(app->window, &actual_width, &actual_height);

    reshape(actual_width, actual_height);

    init_camera(&(app->camera));
    init_scene(&(app->scene));

    app->menu_open = false;
    app->time_paused = false;
    app->time_slider = 60.0f;
    app->fire_brightness = 1.0f;
    app->fog_density = 0.0175f;
    app->dragging_slider = 0;

    app->is_running = true;
}

void init_opengl() {

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glClearColor(0.529, 0.808, 0.922, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP2);
    glFogf(GL_FOG_DENSITY, 0.0175f);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void reshape(GLsizei width, GLsizei height) {

    double ratio = (double)width / height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-.08 * ratio, .08 * ratio, -.08, .08, .1, 1000.0);
}

static void lerp_color(float* out, const float* a, const float* b, float t) {
    out[0] = a[0] + (b[0] - a[0]) * t;
    out[1] = a[1] + (b[1] - a[1]) * t;
    out[2] = a[2] + (b[2] - a[2]) * t;
}

static void draw_celestial_body(float x, float y, float z, float r, float g, float b, float radius) {

    glPushMatrix();
    glTranslatef(x, y, z);

    float modelview[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview);

    modelview[0] = 1.0f; modelview[1] = 0.0f; modelview[2] = 0.0f;
    modelview[4] = 0.0f; modelview[5] = 1.0f; modelview[6] = 0.0f;
    modelview[8] = 0.0f; modelview[9] = 0.0f; modelview[10] = 1.0f;

    glLoadMatrixf(modelview);

    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_FOG);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int segments = 32;
    float pi2 = (float)(2.0 * M_PI);

    glColor4f(r, g, b, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);

    for (int i = 0; i <= segments; i++) {

        float a = i * pi2 / segments;
        glVertex3f(cosf(a) * radius, sinf(a) * radius, 0);
    }
    glEnd();

    float gr = r * 1.2f > 1.0f ? 1.0f : r * 1.2f;
    float gg = g * 1.2f > 1.0f ? 1.0f : g * 1.2f;
    float gb = b * 1.2f > 1.0f ? 1.0f : b * 1.2f;

    glColor4f(gr, gg, gb, 0.15f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);

    for (int i = 0; i <= segments; i++) {

        float a = i * pi2 / segments;
        glVertex3f(cosf(a) * radius * 2.2f, sinf(a) * radius * 2.2f, 0);
    }
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_FOG);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
}

// Updating the color of the sky to match the time of day

static void update_sky(double uptime) {

    double day_phase = fmod(uptime, DAY_DURATION) / DAY_DURATION;

    float sun_angle  = (float)(day_phase * 2.0 * M_PI - M_PI_2);
    float moon_angle = sun_angle + (float)M_PI;

    float orbit = 180.0f;

    float sun_x  =  cosf(sun_angle)  * orbit;
    float sun_z  =  sinf(sun_angle)  * orbit;
    float moon_x =  cosf(moon_angle) * orbit;
    float moon_z =  sinf(moon_angle) * orbit;

    float light_pos[] = { sun_x, 0.0f, sun_z, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    float sky_day[]     = { 0.529f, 0.808f, 0.922f };
    float sky_sunrise[] = { 0.95f,  0.45f,  0.15f  };
    float sky_night[]   = { 0.02f,  0.02f,  0.08f  };

    float sun_diffuse_day[]     = { 1.0f,  0.95f, 0.85f, 1.0f };
    float sun_diffuse_sunrise[] = { 1.0f,  0.55f, 0.2f,  1.0f };
    float sun_diffuse_night[]   = { 0.05f, 0.05f, 0.15f, 1.0f };

    float ambient_day[]   = { 0.35f, 0.35f, 0.35f, 1.0f };
    float ambient_night[] = { 0.03f, 0.03f, 0.08f, 1.0f };

    float sky[3], diffuse[4], ambient[4];
    float t;

    if (day_phase < 0.20f) {

        lerp_color(sky, sky_night, sky_night, 0.0f);

        for (int i = 0; i < 4; i++) diffuse[i] = sun_diffuse_night[i];
        for (int i = 0; i < 4; i++) ambient[i] = ambient_night[i];

    } else if (day_phase < 0.30f) {

        t = (float)((day_phase - 0.20) / 0.10);
        lerp_color(sky, sky_night, sky_sunrise, t);

        for (int i = 0; i < 3; i++) diffuse[i] = sun_diffuse_night[i] + (sun_diffuse_sunrise[i] - sun_diffuse_night[i]) * t;
        diffuse[3] = 1.0f;

        for (int i = 0; i < 3; i++) ambient[i] = ambient_night[i] + (ambient_day[i] - ambient_night[i]) * t;
        ambient[3] = 1.0f;

    } else if (day_phase < 0.40f) {

        t = (float)((day_phase - 0.30) / 0.10);
        lerp_color(sky, sky_sunrise, sky_day, t);

        for (int i = 0; i < 3; i++) diffuse[i] = sun_diffuse_sunrise[i] + (sun_diffuse_day[i] - sun_diffuse_sunrise[i]) * t;
        diffuse[3] = 1.0f;

        for (int i = 0; i < 4; i++) ambient[i] = ambient_day[i];

    } else if (day_phase < 0.70f) {

        lerp_color(sky, sky_day, sky_day, 0.0f);

        for (int i = 0; i < 4; i++) diffuse[i] = sun_diffuse_day[i];
        for (int i = 0; i < 4; i++) ambient[i] = ambient_day[i];

    } else if (day_phase < 0.80f) {

        t = (float)((day_phase - 0.70) / 0.10);
        lerp_color(sky, sky_day, sky_sunrise, t);

        for (int i = 0; i < 3; i++) diffuse[i] = sun_diffuse_day[i] + (sun_diffuse_sunrise[i] - sun_diffuse_day[i]) * t;
        diffuse[3] = 1.0f;

        for (int i = 0; i < 3; i++) ambient[i] = ambient_day[i] + (ambient_night[i] - ambient_day[i]) * t;
        ambient[3] = 1.0f;

    } else if (day_phase < 0.90f) {

        t = (float)((day_phase - 0.80) / 0.10);
        lerp_color(sky, sky_sunrise, sky_night, t);

        for (int i = 0; i < 3; i++) diffuse[i] = sun_diffuse_sunrise[i] + (sun_diffuse_night[i] - sun_diffuse_sunrise[i]) * t;
        diffuse[3] = 1.0f;

        for (int i = 0; i < 4; i++) ambient[i] = ambient_night[i];

    } else {

        lerp_color(sky, sky_night, sky_night, 0.0f);

        for (int i = 0; i < 4; i++) diffuse[i] = sun_diffuse_night[i];
        for (int i = 0; i < 4; i++) ambient[i] = ambient_night[i];
    }

    glClearColor(sky[0], sky[1], sky[2], 1.0f);

    float fog_color[] = { sky[0], sky[1], sky[2], 1.0f };
    glFogfv(GL_FOG_COLOR, fog_color);

    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, diffuse);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    if (sun_z > -20.0f) {

        float sr = diffuse[0] * 1.5f > 1.0f ? 1.0f : diffuse[0] * 1.5f;
        draw_celestial_body(sun_x, 0.0f, sun_z, sr, diffuse[1], diffuse[2] * 0.3f, 8.0f);
    }

    if (moon_z > -20.0f) {
        draw_celestial_body(moon_x, 0.0f, moon_z, 0.85f, 0.88f, 0.95f, 5.0f);
    }
}

// Events - movement, controls, HUD etc.

void handle_app_events(App* app) {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                app->is_running = false;
                break;
            case SDL_SCANCODE_TAB:
                app->menu_open = !app->menu_open;
                SDL_SetRelativeMouseMode(app->menu_open ? SDL_FALSE : SDL_TRUE);
                break;
            case SDL_SCANCODE_W:
                set_camera_speed(&(app->camera), 5); 
                break;
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), -5);
                break;
            case SDL_SCANCODE_A:
                set_camera_side_speed(&(app->camera), 5);
                break;
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), -5);
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), 0);
                break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), 0);
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT && app->menu_open) {

                int mx = event.button.x;
                int my = event.button.y;

                // Slider drag start

                if (my >= 115 && my <= 145 && mx >= 320 && mx <= 780) {
                    app->dragging_slider = 1;
                } else if (my >= 175 && my <= 205 && mx >= 320 && mx <= 780) {
                    app->dragging_slider = 2;
                } else if (my >= 235 && my <= 265 && mx >= 320 && mx <= 780) {
                    app->dragging_slider = 3;
                }

                // Pause checkbox (y: 80-100, x: 20-40)

                if (my >= 80 && my <= 100 && mx >= 20 && mx <= 40) {
                    app->time_paused = !app->time_paused;
                }
            }
            if (event.button.button == SDL_BUTTON_RIGHT && !app->menu_open) {
                scene_interact(&(app->scene), &(app->camera), &(app->scene.uptime));
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                app->dragging_slider = 0;
            }
            break;
        case SDL_MOUSEMOTION:
            // Slider drag while left button held

            if (app->dragging_slider != 0 && app->menu_open) {

                int mx = event.motion.x;
                float t = (float)(mx - 320) / 460.0f;

                if (t < 0.0f) t = 0.0f;
                if (t > 1.0f) t = 1.0f;
                if (app->dragging_slider == 1) {
                    app->time_slider = t * 120.0f;
                    if (!app->time_paused) app->scene.uptime = app->time_slider;
                } else if (app->dragging_slider == 2) {
                    app->fire_brightness = t * 2.0f;
                    app->scene.fire_brightness = app->fire_brightness;
                } else if (app->dragging_slider == 3) {
                    app->fog_density = t * 0.05f;
                    glFogf(GL_FOG_DENSITY, app->fog_density);
                }
            } else if (!app->menu_open) {
                rotate_camera(&(app->camera), -event.motion.xrel * 0.1, -event.motion.yrel * 0.1);
            }
            break;
        case SDL_QUIT:
            app->is_running = false;
            break;
        default:
            break;
        }
    }
}

void update_app(App* app) {
    
    double current_time;
    double elapsed_time;

    current_time = (double)SDL_GetTicks() / 1000.0;
    elapsed_time = current_time - app->uptime;

    if (elapsed_time > 0.1) elapsed_time = 0.1;
    if (elapsed_time < 0) elapsed_time = 0;

    app->uptime = current_time;

    if (app->time_paused) {

        app->scene.uptime = app->time_slider;
        elapsed_time = 0.0;
    }

    vec3 positions[MAX_OBJECTS + 4];
    float radii[MAX_OBJECTS + 4];
    int count = app->scene.num_objects;

    for (int i = 0; i < count; i++) {

        positions[i] = app->scene.objects[i].position;
        radii[i]     = app->scene.objects[i].radius;
    }

    // The hitboxes of the tents and the campfire

    int extra = 0;
    positions[count + extra].x = CAMPFIRE_X;
    positions[count + extra].y = CAMPFIRE_Y;
    positions[count + extra].z = 0.0f;

    radii[count + extra] = app->scene.campfire_radius;
    extra++;

    for (int i = 0; i < 3; i++) {

        positions[count + extra] = app->scene.tent_positions[i];
        radii[count + extra]     = app->scene.tent_radius;
        extra++;
    }

    update_camera(&(app->camera), elapsed_time,
                  positions, radii, count + extra,
                  LAKE_CENTER_X, LAKE_CENTER_Y, 15.0f);

    update_scene(&(app->scene), elapsed_time);
}

void render_app(App* app) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
        set_view(&(app->camera));
        update_sky(app->scene.uptime);
        render_scene(&(app->scene), &(app->camera));
    glPopMatrix();

    if (!app->menu_open) {
        render_crosshair();
    }
    
    if (app->menu_open) {
        render_menu(app);
    }
    
    SDL_GL_SwapWindow(app->window);
}

void render_crosshair() {

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING); 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

    glBegin(GL_LINES);
        glVertex2f(-0.02f, 0.0f);
        glVertex2f(0.02f, 0.0f);
        glVertex2f(0.0f, -0.02f);
        glVertex2f(0.0f, 0.02f);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}


// Render text using the bitmap font. scale: 1 = 8x8 px, 2 = 16x16 px etc.

static void draw_text(const char* text, float x, float y, float r, float g, float b) {

    const int scale = 2;  

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(r, g, b, 1.0f);

    float cx = x;
    for (int ci = 0; text[ci] != '\0'; ci++) {

        unsigned char c = (unsigned char)text[ci];
        if (c > 127) c = '?';

        for (int row = 0; row < 8; row++) {

            unsigned char bits = FONT8X8[c][row];
            for (int col = 0; col < 8; col++) {

                if (bits & (1 << col)) {

                    float px = cx + col * scale;
                    float py = y  + row * scale;

                    glBegin(GL_QUADS);
                        glVertex2f(px,          py);
                        glVertex2f(px + scale,  py);
                        glVertex2f(px + scale,  py + scale);
                        glVertex2f(px,          py + scale);
                    glEnd();
                }
            }
        }

        // Character spacing

        cx += 8 * scale + scale; 
    }
}

void render_menu(App* app) {

    int w_width, w_height;
    SDL_GetWindowSize(app->window, &w_width, &w_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w_width, w_height, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Panel dimensions 

    float px = 10, py = 10, pw = 800, ph = 550;
    float inner_l = 20, inner_r = pw - 10;
    float slider_x = 320, slider_w = 460;

    // Background panel

    glColor4f(0.1f, 0.1f, 0.15f, 0.85f);
    glBegin(GL_QUADS);
        glVertex2f(px,      py);
        glVertex2f(px + pw, py);
        glVertex2f(px + pw, py + ph);
        glVertex2f(px,      py + ph);
    glEnd();

    // Border

    glColor4f(0.4f, 0.6f, 0.8f, 1.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(px,      py);
        glVertex2f(px + pw, py);
        glVertex2f(px + pw, py + ph);
        glVertex2f(px,      py + ph);
    glEnd();

    // Header background

    glColor4f(0.2f, 0.3f, 0.4f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(inner_l, 20);
        glVertex2f(inner_r, 20);
        glVertex2f(inner_r, 50);
        glVertex2f(inner_l, 50);
    glEnd();

    draw_text("Camping Project - Settings", 30, 27, 0.85f, 0.95f, 1.0f);

    // Stop time checkbox

    glColor4f(0.3f, 0.3f, 0.35f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(20, 80);
        glVertex2f(40, 80);
        glVertex2f(40, 100);
        glVertex2f(20, 100);
    glEnd();

    if (app->time_paused) {

        glColor4f(0.4f, 0.8f, 0.4f, 1.0f);
        glBegin(GL_QUADS);
            glVertex2f(25, 85);
            glVertex2f(35, 85);
            glVertex2f(35, 95);
            glVertex2f(25, 95);
        glEnd();
    }

    draw_text("Stop time", 50, 78, 0.9f, 0.9f, 0.9f);

    // Time of day slider

    draw_text("Time of day:", 30, 120, 0.8f, 0.85f, 0.9f);

    glColor4f(0.3f, 0.3f, 0.35f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(slider_x,             120);
        glVertex2f(slider_x + slider_w,  120);
        glVertex2f(slider_x + slider_w,  140);
        glVertex2f(slider_x,             140);
    glEnd();

    float time_pos = (app->time_slider / 120.0f) * slider_w;

    glColor4f(0.5f, 0.7f, 0.9f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(slider_x + time_pos - 5, 115);
        glVertex2f(slider_x + time_pos + 5, 115);
        glVertex2f(slider_x + time_pos + 5, 145);
        glVertex2f(slider_x + time_pos - 5, 145);
    glEnd();

    // Fire brightness slider

    draw_text("Fire brightness:", 30, 180, 0.8f, 0.85f, 0.9f);

    glColor4f(0.3f, 0.3f, 0.35f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(slider_x,             180);
        glVertex2f(slider_x + slider_w,  180);
        glVertex2f(slider_x + slider_w,  200);
        glVertex2f(slider_x,             200);
    glEnd();

    float fire_pos = (app->fire_brightness / 2.0f) * slider_w;

    glColor4f(1.0f, 0.5f, 0.1f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(slider_x + fire_pos - 5, 175);
        glVertex2f(slider_x + fire_pos + 5, 175);
        glVertex2f(slider_x + fire_pos + 5, 205);
        glVertex2f(slider_x + fire_pos - 5, 205);
    glEnd();

    // Fog density slider

    draw_text("Fog density:", 30, 240, 0.8f, 0.85f, 0.9f);

    glColor4f(0.3f, 0.3f, 0.35f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(slider_x,             240);
        glVertex2f(slider_x + slider_w,  240);
        glVertex2f(slider_x + slider_w,  260);
        glVertex2f(slider_x,             260);
    glEnd();

    float fog_pos = (app->fog_density / 0.05f) * slider_w;

    glColor4f(0.6f, 0.65f, 0.7f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(slider_x + fog_pos - 5, 235);
        glVertex2f(slider_x + fog_pos + 5, 235);
        glVertex2f(slider_x + fog_pos + 5, 265);
        glVertex2f(slider_x + fog_pos - 5, 265);
    glEnd();

    // Controls section header

    glColor4f(0.2f, 0.3f, 0.4f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(inner_l, 320);
        glVertex2f(inner_r, 320);
        glVertex2f(inner_r, 350);
        glVertex2f(inner_l, 350);
    glEnd();

    draw_text("Controls", 30, 325, 0.85f, 0.95f, 1.0f);

    float control_y = 370;
    const char* control_labels[] = {
        "W / S        - Move forward / backward",
        "A / D        - Move left / right",
        "Mouse        - Look around",
        "Right click  - Interact",
        "TAB          - Open / close menu",
        "ESC          - Quit",
        "       "
    };

    for (int i = 0; i < 7; i++) {

        if (i % 2 == 0) {
            
            glColor4f(0.15f, 0.15f, 0.2f, 0.5f);
            glBegin(GL_QUADS);
                glVertex2f(30,       control_y);
                glVertex2f(inner_r,  control_y);
                glVertex2f(inner_r,  control_y + 25);
                glVertex2f(30,       control_y + 25);
            glEnd();
        }
        draw_text(control_labels[i], 40, control_y + 4, 0.85f, 0.85f, 0.9f);
        control_y += 25;
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
void destroy_app(App* app){

    if (app->gl_context != NULL) {
        SDL_GL_DeleteContext(app->gl_context);
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
    }

    SDL_Quit();
}