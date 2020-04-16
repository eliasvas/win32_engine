#pragma once
#include "string.h" //for memcpy
#include "help.h"
#include "shader.h"
#include "texture.h"
#include "quad.h"
#include "ext/HandmadeMath.h"
#define SHADER_MAX 32
#define RECT_MAX 32
#define TEXTURE_MAX 32
#define SPRITE_MAX 32
#define DIAMOND_SIZE sizeof(float)

struct rect 
{
    v2 pos;
    v2 scale;
    f32 color;
};
#define RECT_SIZE sizeof(rect)

struct sprite 
{
    v2 pos;
    v2 scale;
    GLuint texture_unit;
    GLfloat opacity;
    //..
};
#define SPRITE_SIZE sizeof(sprite)

struct renderer
{
    GLuint rect_vao, vertex_vbo, rect_instance_vbo;

    GLuint sprite_vao, sprite_instance_vbo;//..

    rect rect_instance_data[RECT_MAX];
    i32 rect_alloc_pos;

    sprite sprite_instance_data[SPRITE_MAX];
    i32 sprite_alloc_pos;
     

    hmm_mat4 projection_matrix;
    u32 render_width, render_height;
    shader shaders[SHADER_MAX];
    texture tex[TEXTURE_MAX];
    i32 tex_count;
};
static void 
init_renderer(renderer* r);

static void
renderer_begin(renderer* rend, f32 w, f32 h);

static void 
renderer_render(renderer* rend,float* proj, GLfloat time = 0.f);

#include "renderer.c"


