#ifndef MODEL_H
#define MODEL_H

#include <GL/gl.h>
#include <stdbool.h>
#include "utils.h"

#define MAX_MATERIALS 8

typedef struct {
    int v, vt, vn;
} FaceVertex;

typedef struct {
    FaceVertex vertices[3];
    int material_index; 
} Face;

typedef struct {
    char name[64];       
    GLuint texture_id;
} Material;

typedef struct {
    vec3* vertices;
    vec3* texcoords;
    vec3* normals;
    Face* faces;
    int n_vertices, n_texcoords, n_normals, n_faces;

    Material materials[MAX_MATERIALS];
    int n_materials;

    GLuint display_list;
} Model;

bool load_model(Model* model, const char* obj_path);
void draw_model(const Model* model);
void free_model(Model* model);

#endif /* MODEL_H */