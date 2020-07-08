#pragma once
#include "string.h" //for memcpy
#include "tools.h"
#include "shader.h"
#include "texture.h"
#include "model.h"
#include "quad.h"
#include "shadowmap.h"
#include "ext/HandmadeMath.h"
#define SHADER_MAX 32
#define RECT_MAX 32
#define TEXTURE_MAX 32
#define SPRITE_MAX 128 
#define LIGHT_MAX 10 
#define MODEL_MAX 128
#define DIAMOND_SIZE sizeof(float)

GLfloat vertices[8] = {0.0f,0.0f,1.0f,0.0f,0.0f,1.0f,1.0f,1.0f};

struct Rect 
{
    vec2 pos;
    vec2 scale;
    f32 color;
};
#define RECT_SIZE sizeof(Rect)

typedef struct OpenGLFBO
{
    GLuint fbo;
    GLuint color_attachments[4];
    GLuint depth_attachment;
    u32 w,h;
} OpenGLFBO;

typedef struct Quad2D 
{
    vec2 pos;
    vec2 scale;
    GLuint texture_unit;
    GLfloat opacity;
    vec2 bl;
    vec2 tex_dim;
    GLuint flip;
} Quad2D;


typedef struct ModelInfo
{
    GLuint vao;
    mat4 model_matrix;
	i32 count;
    b32 indexed;

}ModelInfo;

#define SPRITE_SIZE sizeof(Quad2D)

struct Renderer
{
    GLuint rect_vao, vertex_vbo, rect_instance_vbo;

    GLuint renderable_vao, renderable_instance_vbo;//..

    Rect rect_instance_data[RECT_MAX];
    u32 rect_alloc_pos;

    Quad2D renderable_instance_data[SPRITE_MAX];
    u32 renderable_alloc_pos;

    ModelInfo meshes[MODEL_MAX];
    u32 mesh_count;


    mat4 projection_matrix;

    u32 render_width, render_height;
    

    Shader shaders[SHADER_MAX];
    Texture tex[TEXTURE_MAX];
    u32 tex_count;
    DirLight dir_lights[LIGHT_MAX];
    u32 dir_light_count;
    PointLight point_lights[LIGHT_MAX];
    u32 point_light_count;



    ShadowMapFBO shadowmap;
    ShadowmapDebugQuad debug_quad;


    mat4 view_matrix;
    mat4 orthographic_projection;
    mat4 perspective_projection;
    vec3 camera_pos;
};
static void 
init_renderer(Renderer* r);

static void
renderer_begin(Renderer* rend, f32 w, f32 h);

static void 
renderer_render(Renderer* rend,float* proj);

static void
renderer_render_scene(Renderer* rend,float* proj);



#include "renderer.c"


