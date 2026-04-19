#ifndef SETTINGS_H
#define SETTINGS_H

/* --- WORLD & PERFORMANCE --- */
#define CHUNK_SIZE          60.0f
#define RENDER_DISTANCE     1
#define MAX_CHUNK_OBJECTS   150
#define LAKE_RADIUS         22.0f
#define LAKE_CENTER_X       18.0f
#define LAKE_CENTER_Y       18.0f
#define MAX_MODELS 25

/* --- PLAYER & CONTROLS --- */
#define PLAYER_WALK_SPEED   20.0f
#define PLAYER_EYE_HEIGHT   1.5f
#define PLAYER_HITBOX_RAD   0.5f
#define MOUSE_SENSITIVITY   0.1f
#define INTERACT_DIST       2.5f

/* --- ENVIRONMENT & TIME --- */
#define DAY_DURATION        120.0   
#define DAY_START_TIME      (DAY_DURATION / 2.0) 

/* --- COLORS & FOG --- */
#define FOG_DENSITY_DEFAULT 0.0175f
#define SKY_DAY_R 0.529f
#define SKY_DAY_G 0.808f
#define SKY_DAY_B 0.922f
#define SKY_NIGHT_R 0.02f
#define SKY_NIGHT_G 0.02f
#define SKY_NIGHT_B 0.08f

#endif /* SETTINGS_H */