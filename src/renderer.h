#pragma once
#include "string.h" //for memcpy
#include "help.h"
#include "shader.h"
#include "texture.h"
#include "quad.h"
#include "ext/HandmadeMath.h"
#define SHADER_MAX 32
#define TEXTURE_MAX 32
#define RECT_MAX 32
#define BYTES_PER_RECT (sizeof(GLfloat) * 1) 
#define DIAMOND_SIZE sizeof(float)

/* //NOTE(ilias): OLD RENDERER
struct rect
{
    hmm_vec2 pos;
    hmm_vec2 scale;
};

struct renderer
{
    i32 render_width, render_height;
    hmm_mat4 projection_matrix;
    u32 VAO;

    GLuint rect_buffer;
    GLuint rect_instance_buffer;

    i32 rect_alloc_pos;
    rect rects[RECT_MAX];
    shader shaders[SHADER_MAX];
    shader s;

};

static void 
init_renderer(renderer* r);

static void
renderer_begin(renderer* rend, f32 w, f32 h);

static void 
renderer_push_rect(renderer* rend, hmm_vec2 pos, hmm_vec2 scale);

static void 
renderer_render(renderer* rend);

#
*/
struct sprite
{
    hmm_vec2 pos;
    hmm_vec2 scale;
    f32 color;
};
struct renderer
{
    GLuint meshVAO, vertexVBO, instanceVBO;
    GLfloat translations[RECT_MAX];
    u32 render_width, render_height;
    i32 rect_alloc_pos;
    hmm_mat4 projection_matrix;
    shader s;
};
static void 
init_renderer(renderer* r);

static void
renderer_begin(renderer* rend, f32 w, f32 h);

static void 
renderer_render(renderer* rend);

static void
renderer_update(renderer* rend);

#include "renderer.c"


