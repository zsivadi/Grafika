#include "model.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL_image.h>

// Texture loading function

GLuint load_texture(const char* filename) {

    SDL_Surface* surface = IMG_Load(filename);

    if (!surface) {

        printf("[ERROR] Texture not found: %s | %s\n", filename, IMG_GetError());
        return 0;
    }

    SDL_Surface* conv = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);

    if (!conv) {

        printf("[ERROR] Surface convertion failed: %s\n", SDL_GetError());
        return 0;
    }

    GLuint tid;

    glGenTextures(1, &tid);
    glBindTexture(GL_TEXTURE_2D, tid);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, conv->w, conv->h,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, conv->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    printf("[INFO] Texture OK: %s (%dx%d)\n", filename, conv->w, conv->h);
    SDL_FreeSurface(conv);
    return tid;
}

// Load the texture by material name

static GLuint load_texture_by_material_name(const char* mat_name) {

    char path[512];
    snprintf(path, sizeof(path), "assets/textures/%s.png", mat_name);

    GLuint tid = load_texture(path);
    if (tid != 0) return tid;

    snprintf(path, sizeof(path), "assets/textures/%s.jpg", mat_name);
    tid = load_texture(path);
    if (tid != 0) return tid;

    return 0;
}

// Fallback:
// If there's no map_Kd try _BaseColor and _Diffuse without suffixes
// Also try mat_name search

static GLuint load_texture_with_fallbacks(const char* tex_filename, const char* mat_name) {

    char path[512];
    snprintf(path, sizeof(path), "assets/textures/%s", tex_filename);

    GLuint tid = load_texture(path);
    if (tid != 0) return tid;

    // Cropping suffix

    const char* suffixes[] = { "_BaseColor", "_Diffuse", "_diffuse", "_albedo", NULL };
    char base[256];
    strncpy(base, tex_filename, 255);
    base[255] = '\0';

    // Cropping file extension

    char* dot = strrchr(base, '.');
    const char* ext = dot ? dot : ".png";
    if (dot) *dot = '\0';

    for (int s = 0; suffixes[s] != NULL; s++) {

        int slen = (int)strlen(suffixes[s]);
        int blen = (int)strlen(base);

        if (blen > slen && strcmp(base + blen - slen, suffixes[s]) == 0) {

            char stripped[256];
            strncpy(stripped, base, blen - slen);
            stripped[blen - slen] = '\0';

            snprintf(path, sizeof(path), "assets/textures/%s%s", stripped, ext);
            tid = load_texture(path);
            if (tid != 0) return tid;
        }
    }

    // Last fallback: try by material name

    return load_texture_by_material_name(mat_name);
}

// Load texture names and Kd colors from .mtl files

static int load_materials(Material* materials, const char* mtl_path) {

    FILE* f = fopen(mtl_path, "r");

    if (!f) {

        printf("[WARN] MTL not found: %s\n", mtl_path);
        return 0;
    }

    int n = 0;
    char line[512];
    int current = -1;

    while (fgets(line, sizeof(line), f) && n < MAX_MATERIALS) {
        
        char* end = line + strlen(line) - 1;
        while (end > line && (*end == '\n' || *end == '\r' || *end == ' ')) *end-- = '\0';

        if (strncmp(line, "newmtl ", 7) == 0) {

            current = n++;
            strncpy(materials[current].name, line + 7, 63);

            materials[current].name[63] = '\0';
            materials[current].texture_id = 0;
            materials[current].kd[0] = 0.8f;
            materials[current].kd[1] = 0.8f;
            materials[current].kd[2] = 0.8f;

        } else if (strncmp(line, "Kd ", 3) == 0 && current >= 0) {

            sscanf(line, "Kd %f %f %f",
                &materials[current].kd[0],
                &materials[current].kd[1],
                &materials[current].kd[2]);

        } else if (strncmp(line, "map_Kd ", 7) == 0 && current >= 0) {
            
            const char* tex_file = line + 7;
            const char* slash  = strrchr(tex_file, '/');
            const char* bslash = strrchr(tex_file, '\\');
            const char* sep = slash > bslash ? slash : bslash;

            if (sep) tex_file = sep + 1;

            materials[current].texture_id = load_texture_with_fallbacks(tex_file, materials[current].name);
        }
    }
    fclose(f);

    for (int i = 0; i < n; i++) {

        if (materials[i].texture_id == 0) {

            materials[i].texture_id = load_texture_by_material_name(materials[i].name);

            if (materials[i].texture_id == 0) {

                printf("[INFO] Material '%s' uses Kd color (%.2f, %.2f, %.2f)\n",
                       materials[i].name,
                       materials[i].kd[0], materials[i].kd[1], materials[i].kd[2]);
            }
        }
    }

    return n;
}

// Loading the models from assets

bool load_model(Model* model, const char* obj_path) {

    FILE* file = fopen(obj_path, "r");
    if (!file) {

        printf("[ERROR] OBJ not found: %s\n", obj_path);
        return false;
    }

    char line[512];

    model->n_vertices = model->n_texcoords = model->n_normals = model->n_faces = 0;
    model->n_materials = 0;
    model->display_list = 0;

    char mtl_filename[256] = "";

    while (fgets(line, sizeof(line), file)) {

        if      (strncmp(line, "v ",  2) == 0) model->n_vertices++;
        else if (strncmp(line, "vt ", 3) == 0) model->n_texcoords++;
        else if (strncmp(line, "vn ", 3) == 0) model->n_normals++;
        else if (strncmp(line, "f ",  2) == 0) model->n_faces += 2;
        else if (strncmp(line, "mtllib ", 7) == 0) sscanf(line, "mtllib %255s", mtl_filename);
    }

    model->vertices  = (vec3*)malloc(model->n_vertices  * sizeof(vec3));
    model->texcoords = (vec3*)malloc(model->n_texcoords * sizeof(vec3));
    model->normals   = (vec3*)malloc(model->n_normals   * sizeof(vec3));
    model->faces     = (Face*)malloc(model->n_faces     * sizeof(Face));

    if (strlen(mtl_filename) > 0) {

        char mtl_path[512];
        snprintf(mtl_path, sizeof(mtl_path), "assets/models/%s", mtl_filename);
        model->n_materials = load_materials(model->materials, mtl_path);
    }

    rewind(file);

    int v_idx = 0, vt_idx = 0, vn_idx = 0, f_idx = 0;
    int current_mat = 0; 

    while (fgets(line, sizeof(line), file)) {

        if (strncmp(line, "v ", 2) == 0) {

            sscanf(line, "v %f %f %f",
                &model->vertices[v_idx].x,
                &model->vertices[v_idx].y,
                &model->vertices[v_idx].z);
            v_idx++;

        } else if (strncmp(line, "vt ", 3) == 0) {

            sscanf(line, "vt %f %f",
                &model->texcoords[vt_idx].x,
                &model->texcoords[vt_idx].y);
            vt_idx++;

        } else if (strncmp(line, "vn ", 3) == 0) {

            sscanf(line, "vn %f %f %f",
                &model->normals[vn_idx].x,
                &model->normals[vn_idx].y,
                &model->normals[vn_idx].z);
            vn_idx++;

        } else if (strncmp(line, "usemtl ", 7) == 0) {

            char mat_name[64];
            sscanf(line, "usemtl %63s", mat_name);
            current_mat = 0;

            for (int i = 0; i < model->n_materials; i++) {

                if (strcmp(model->materials[i].name, mat_name) == 0) {

                    current_mat = i;
                    break;
                }
            }

        } else if (strncmp(line, "f ", 2) == 0) {

            FaceVertex poly[8];
            int poly_n = 0;
            char* ptr = line + 2;

            while (poly_n < 8) {

                FaceVertex fv = {0, 0, 0};
                int c = 0;

                if      (sscanf(ptr, "%d/%d/%d%n", &fv.v, &fv.vt, &fv.vn, &c) == 3) {}
                else if (sscanf(ptr, "%d//%d%n",   &fv.v, &fv.vn, &c)      == 2) {}
                else if (sscanf(ptr, "%d/%d%n",    &fv.v, &fv.vt, &c)      == 2) {}
                else if (sscanf(ptr, "%d%n",       &fv.v, &c)               == 1) {}
                else break;

                poly[poly_n++] = fv;
                ptr += c;

                while (*ptr == ' ' || *ptr == '\t') ptr++;
                if (*ptr == '\n' || *ptr == '\r' || *ptr == '\0') break;
            }

            for (int t = 1; t + 1 < poly_n && f_idx < model->n_faces; t++) {

                model->faces[f_idx].vertices[0] = poly[0];
                model->faces[f_idx].vertices[1] = poly[t];
                model->faces[f_idx].vertices[2] = poly[t + 1];
                model->faces[f_idx].material_index = current_mat;

                f_idx++;
            }
        }
    }
    fclose(file);

    model->display_list = glGenLists(1);
    glNewList(model->display_list, GL_COMPILE);

    int mat_count = model->n_materials > 0 ? model->n_materials : 1;

    for (int m = 0; m < mat_count; m++) {

        GLuint tid = (model->n_materials > 0) ? model->materials[m].texture_id : 0;

        if (tid != 0) {

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, tid);
            glEnable(GL_ALPHA_TEST);
            glAlphaFunc(GL_GREATER, 0.3f);
            glColor3f(1.0f, 1.0f, 1.0f);

        } else {

            glDisable(GL_TEXTURE_2D);
            glDisable(GL_ALPHA_TEST);

            if (model->n_materials > 0) {
                
                glColor3f(model->materials[m].kd[0],
                          model->materials[m].kd[1],
                          model->materials[m].kd[2]);
            }
        }

        glBegin(GL_TRIANGLES);

        for (int i = 0; i < f_idx; i++) {

            if (model->faces[i].material_index != m) continue;
            for (int j = 0; j < 3; j++) {

                FaceVertex fv = model->faces[i].vertices[j];
                if (fv.vn > 0 && fv.vn <= model->n_normals)
                    glNormal3f(model->normals[fv.vn-1].x,
                               model->normals[fv.vn-1].y,
                               model->normals[fv.vn-1].z);

                if (fv.vt > 0 && fv.vt <= model->n_texcoords)
                    glTexCoord2f(model->texcoords[fv.vt-1].x,
                                 1.0f - model->texcoords[fv.vt-1].y);

                if (fv.v  > 0 && fv.v  <= model->n_vertices)
                    glVertex3f(model->vertices[fv.v-1].x,
                               model->vertices[fv.v-1].y,
                               model->vertices[fv.v-1].z);
            }
        }
        glEnd();
    }

    glDisable(GL_ALPHA_TEST);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEndList();

    free(model->vertices);  model->vertices  = NULL;
    free(model->texcoords); model->texcoords = NULL;
    free(model->normals);   model->normals   = NULL;
    free(model->faces);     model->faces     = NULL;

    printf("[INFO] Model OK: %s (%d tri, %d material)\n",
           obj_path, f_idx, model->n_materials);
    return true;
}

void draw_model(const Model* model) {
    glCallList(model->display_list);
}

void free_model(Model* model) {

    if (model->display_list) {

        glDeleteLists(model->display_list, 1);
        model->display_list = 0;
    }
    for (int i = 0; i < model->n_materials; i++) {

        if (model->materials[i].texture_id) {

            glDeleteTextures(1, &model->materials[i].texture_id);
            model->materials[i].texture_id = 0;
        }
    }
}