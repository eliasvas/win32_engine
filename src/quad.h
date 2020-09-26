#ifndef QUAD_H
#define QUAD_H

#include "texture.h"
#include "tools.h"
#include "shader.h"


static f32 quad_vertices[] = {
     // positions          // texture coords
     25.f,  25.f, 0.0f,   1.0f, 1.0f,   // top right
     25.f, -25.f, 0.0f,   1.0f, 0.0f,   // bottom right
    -25.f, -25.f, 0.0f,   0.0f, 0.0f,   // bottom left
    -25.f,  25.f, 0.0f,   0.0f, 1.0f    // top left
};


static u32 quad_indices[] = {  // note that we start from 0!
    0, 3, 2,  // first Triangle
    2, 1, 0   // second Triangle
};

 
typedef struct quad
{
    vec3 pos;
    u32 VAO;
    Texture t;
    Shader s;
}quad;

/*This part is for rendering the colliders as line strips, 
 * maybe TODO change this to separate struct??*/
static GLfloat vertex_data[] = {
                            0.0f,0.0f,
                            1.0f,0.0f,
                            1.0f,1.0f,
                            0.0f,1.0f,
                            };

void 
init_quad(quad* q);

void 
init_quad(quad* q, const char * tex);

void 
render_quad(quad* q, float* m);

void 
render_quad(quad* q, mat4 m);


void 
render_collider(quad* q, float* m,f32 red = 0.f);

#endif
