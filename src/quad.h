#pragma once
#include "tb.h"
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

 
struct quad
{
    vec3 pos;
    u32 VAO;
    Texture t;
    Shader s;
};

static void 
init_quad(quad* q, const char * tex)
{
    GLuint VBO, EBO;
    q->pos = {0.0f,0.0f,0.0f};
    shader_load(&q->s,"../assets/shaders/textured_quad.vert", "../assets/shaders/textured_quad.frag");
    glGenVertexArrays(1, &q->VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(q->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    load_texture(&q->t,tex);
    glBindVertexArray(0);
}

/*This part is for rendering the colliders as line strips, 
 * maybe TODO change this to separate struct??*/
GLfloat vertex_data[] = {
                            0.0f,0.0f,
                            1.0f,0.0f,
                            1.0f,1.0f,
                            0.0f,1.0f,
                            };

static void 
init_quad(quad* q)
{
    GLuint VBO;
    shader_load(&q->s,"../assets/shaders/lines.vert", "../assets/shaders/lines.frag");
    glGenVertexArrays(1, &q->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(q->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}



static void 
render_quad(quad* q, float* m)
{
    use_shader(&q->s);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, q->t.id);
    setMat4fv(&q->s, "MVP", m);
    glBindVertexArray(q->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

/*
static void 
render_quad(quad* q, float* m,f32 red = 0.f)
{
    use_shader(&q->s);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, q->t.id);
    setMat4fv(&q->s, "MVP", m);
    glBindVertexArray(q->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
*/
static void 
render_collider(quad* q, float* m,f32 red = 0.f)
{
    use_shader(&q->s);
    setMat4fv(&q->s, "MVP", m);
    setFloat(&q->s, "red", red);
    glBindVertexArray(q->VAO);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glBindVertexArray(0);
}


