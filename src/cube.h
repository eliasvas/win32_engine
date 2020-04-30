#pragma once
#include "ext/HandmadeMath.h" 
#include "shader.h"
#include "tb.h"

struct Cube
{
    GLuint VAO;
    vec3 center;
    Shader cube_shader; //maybe put it ouside the struct or make it static(???)
};

static void 
init_cube(Cube* c)
{
    shader_load(&c->cube_shader,"../assets/shaders/colored_cube.vert", "../assets/shaders/colored_cube.frag");
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  1.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  1.0f, 1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f
    };

    unsigned int VBO;
    glGenVertexArrays(1, &c->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(c->VAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute    //NOTE(ilias):maybe update face normals after transform??
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

static void
render_cube(Cube* c, mat4 projection_matrix)
{
    use_shader(&c->cube_shader);

    mat4 world_matrix = translate_mat4(c->center);
    mat4 MVP = mul_mat4(projection_matrix,world_matrix);


    auto loc = glGetUniformLocation(c->cube_shader.ID, "MVP");
    glUniformMatrix4fv(loc,1, GL_FALSE, (GLfloat*)MVP.elements);
    glBindVertexArray(c->VAO);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}






