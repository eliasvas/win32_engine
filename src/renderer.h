#pragma once
#include "string.h" //for memcpy
#include "tb.h"
#include "shader.h"
#include "texture.h"
#include "quad.h"
#include "ext/HandmadeMath.h"
#define SHADER_MAX 32
#define RECT_MAX 32
#define TEXTURE_MAX 32
#define SPRITE_MAX 128 
#define DIAMOND_SIZE sizeof(float)

GLfloat vertices[8] = {0.0f,0.0f,1.0f,0.0f,0.0f,1.0f,1.0f,1.0f};

struct Rect 
{
    vec2 pos;
    vec2 scale;
    f32 color;
};
#define RECT_SIZE sizeof(Rect)

struct OpenGLFBO
{
    GLuint fbo;
    GLuint color_attachments[4];
    GLuint depth_attachment;
    u32 w,h;
};

struct Renderable 
{
    vec2 pos;
    vec2 scale;
    GLuint texture_unit;
    GLfloat opacity;
    vec2 bl;
    vec2 tex_dim;
    GLuint flip;
};
#define SPRITE_SIZE sizeof(Renderable)

struct renderer
{
    GLuint rect_vao, vertex_vbo, rect_instance_vbo;

    GLuint renderable_vao, renderable_instance_vbo;//..

    Rect rect_instance_data[RECT_MAX];
    i32 rect_alloc_pos;

    Renderable renderable_instance_data[SPRITE_MAX];
    i32 renderable_alloc_pos;
     

    mat4 projection_matrix;
    u32 render_width, render_height;
    Shader shaders[SHADER_MAX];
    Texture tex[TEXTURE_MAX];
    i32 tex_count;


    mat4 view_projection;
    mat4 orthographic_projection;
    mat4 perspective_projection;
};
static void 
init_renderer(renderer* r);

static void
renderer_begin(renderer* rend, f32 w, f32 h);

static void 
renderer_render(renderer* rend,float* proj);

#include "renderer.c"


