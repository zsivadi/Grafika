#ifndef APP_H
#define APP_H

#include <GL/gl.h>
#include "camera.h"
#include "scene.h"

#include <SDL2/SDL.h>
#include <stdbool.h>

#define VIEWPORT_RATIO (4.0 / 3.0)

#define DAY_DURATION 120.0

typedef struct App {
    SDL_Window* window;
    SDL_GLContext gl_context;
    bool is_running;
    double uptime;
    Camera camera;
    Scene scene;
    
    bool menu_open;
    bool time_paused;
    float time_slider;
    float fire_brightness;
    float fog_density;
    int dragging_slider;
} App;

// Initialize the application window, OpenGL context, and game systems

void init_app(App* app, int width, int height);

// Configure OpenGL state including lighting, fog, and rendering modes

void init_opengl();

// Handle window resize and update viewport/projection matrix

void reshape(GLsizei width, GLsizei height);

// Process SDL events including keyboard input, mouse movement, and menu interactions

void handle_app_events(App* app);

// Update application state including camera movement, collision detection, and scene updates

void update_app(App* app);

// Render the complete frame including 3D scene, UI elements, and menu

void render_app(App* app);

// Clean up resources and shut down SDL/OpenGL systems

void destroy_app(App* app);

// Render the player's crosshair in screen space

void render_crosshair();

// Render the menu with settings controls and key bindings information

void render_menu(App* app);

#endif /* APP_H */