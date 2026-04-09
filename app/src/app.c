#include "app.h"

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

    glColor4f(r * 1.2f > 1.0f ? 1.0f : r * 1.2f,
              g * 1.2f > 1.0f ? 1.0f : g * 1.2f,
              b * 1.2f > 1.0f ? 1.0f : b * 1.2f,
              0.15f);

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

static void update_sky(double uptime, float* out_sun_x, float* out_sun_y, float* out_sun_z) {

    double day_phase = fmod(uptime, DAY_DURATION) / DAY_DURATION;

    float sun_angle  = (float)(day_phase * 2.0 * M_PI - M_PI_2);
    float moon_angle = sun_angle + (float)M_PI;

    float orbit = 180.0f;

    float sun_x =  cosf(sun_angle) * orbit;
    float sun_z =  sinf(sun_angle) * orbit;
    float moon_x = cosf(moon_angle) * orbit;
    float moon_z = sinf(moon_angle) * orbit;

    if (out_sun_x) *out_sun_x = sun_x;
    if (out_sun_y) *out_sun_y = 0.0f;
    if (out_sun_z) *out_sun_z = sun_z;

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

    float sun_alpha  = sun_z  > -20.0f ? 1.0f : 0.0f;
    float moon_alpha = moon_z > -20.0f ? 1.0f : 0.0f;

    if (sun_alpha > 0.0f) {

        float sun_r = diffuse[0] * 1.5f > 1.0f ? 1.0f : diffuse[0] * 1.5f;
        float sun_g = diffuse[1];
        float sun_b = diffuse[2] * 0.3f;
        draw_celestial_body(sun_x, 0.0f, sun_z, sun_r, sun_g, sun_b, 8.0f);
    }

    if (moon_alpha > 0.0f) {
        draw_celestial_body(moon_x, 0.0f, moon_z, 0.85f, 0.88f, 0.95f, 5.0f);
    }
}

void handle_app_events(App* app) {

    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_KEYDOWN:
            switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                app->is_running = false;
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
        case SDL_MOUSEMOTION:
            rotate_camera(&(app->camera), -event.motion.xrel * 0.1, -event.motion.yrel * 0.1);
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

    vec3 positions[MAX_OBJECTS];
    float radii[MAX_OBJECTS];
    int count = app->scene.num_objects;

    for (int i = 0; i < count; i++) {
        positions[i] = app->scene.objects[i].position;
        radii[i]     = app->scene.objects[i].radius;
    }

    update_camera(&(app->camera), elapsed_time,
                  positions, radii, count,
                  LAKE_CENTER_X, LAKE_CENTER_Y, 15.0f);

    update_scene(&(app->scene), elapsed_time);
}

void render_app(App* app) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
        set_view(&(app->camera));
        update_sky(app->scene.uptime, NULL, NULL, NULL);
        render_scene(&(app->scene), &(app->camera));
    glPopMatrix();

    render_crosshair();
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

void destroy_app(App* app){

    if (app->gl_context != NULL) {
        SDL_GL_DeleteContext(app->gl_context);
    }

    if (app->window != NULL) {
        SDL_DestroyWindow(app->window);
    }

    SDL_Quit();
}