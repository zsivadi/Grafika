#include "app.h"

#include <GL/gl.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>

void init_app(App* app, int width, int height) {
    
    int error_code;
    int inited_loaders;

    app->is_running = false;

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

    SDL_SetRelativeMouseMode(SDL_TRUE);
        
    if (app->window == NULL) {
        printf("[ERROR] Unable to create the application window!\n");
        return;
    }

    inited_loaders = IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    if (inited_loaders == 0) {
        printf("[ERROR] IMG initialization error: %s\n", IMG_GetError());
        return;
    }

    app->gl_context = SDL_GL_CreateContext(app->window);
    if (app->gl_context == NULL) {
        printf("[ERROR] Unable to create the OpenGL context!\n");
        return;
    }

    int actual_width, actual_height;
    SDL_GetWindowSize(app->window, &actual_width, &actual_height);

    init_opengl();
    reshape(actual_width, actual_height);

    init_camera(&(app->camera));
    init_scene(&(app->scene));

    app->is_running = true;
}

void init_opengl() {

    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);
    glEnable(GL_AUTO_NORMAL);

    glClearColor(0.05, 0.05, 0.1, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);

    glEnable(GL_TEXTURE_2D);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void reshape(GLsizei width, GLsizei height) {

    double ratio = (double)width / height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-.08 * ratio, .08 * ratio, -.08, .08, .1, 1000.0);
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
                set_camera_speed(&(app->camera), 3); 
                break;
            case SDL_SCANCODE_S:
                set_camera_speed(&(app->camera), -3);
                break;
            case SDL_SCANCODE_A:
                set_camera_side_speed(&(app->camera), 3);
                break;
            case SDL_SCANCODE_D:
                set_camera_side_speed(&(app->camera), -3);
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
    app->uptime = current_time;

    update_camera(&(app->camera), elapsed_time);
    update_scene(&(app->scene), elapsed_time);
}

void render_app(App* app) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);

    glPushMatrix();
        set_view(&(app->camera));

        float light_position[] = { 0.0f, 0.0f, 50.0f, 1.0f }; 
        glLightfv(GL_LIGHT0, GL_POSITION, light_position);

        render_scene(&(app->scene));
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