#pragma once
#include "ext/HandmadeMath.h" 
#include "shader.h"
#include "Model.h"
#include "tools.h"

typedef struct Cube
{
    GLuint vao;
    vec3 center;
    Shader cube_shader; //maybe put it ouside the struct or make it static(???)
    Texture diff;
    Texture spec;
}Cube;
static void 
init_cube_textured(Cube* c)
{
    shader_load(&c->cube_shader,"../assets/shaders/phong_tex.vert", "../assets/shaders/phong_tex.frag");
float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    load_texture(&(c->diff),"../assets/dirt.png");
    load_texture(&(c->spec),"../assets/noise.png");
        
    u32 vbo;
    glGenVertexArrays(1, &c->vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(c->vao);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute    //NOTE(ilias):maybe update face normals after transform??
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
     // texcoords attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    
    glBindVertexArray(0);
}


static void 
render_cube_textured(Cube* c, mat4* projection, mat4* view, vec3 light_pos, vec3 camera_pos)
{
    use_shader(&c->cube_shader);


    setInt(&c->cube_shader, "m.diffuse", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, c->diff.id);
    setInt(&c->cube_shader, "m.specular", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, c->spec.id);

    Light light = {light_pos,{0.2f, 0.2f, 0.2f},{0.7f, 0.7f, 0.7f},{1.0f, 1.0f, 1.0f}};
    mat4 model = translate_mat4(c->center);
    setMat4fv(&c->cube_shader, "proj", (GLfloat*)projection->elements);
    setMat4fv(&c->cube_shader, "view", (GLfloat*)view->elements);
    setMat4fv(&c->cube_shader, "model", (GLfloat*)model.elements);
    {
        setVec3(&c->cube_shader,"m.ambient", {0.2f, 0.2f, 0.2f});
        setFloat(&c->cube_shader, "m.shininess", 3.f);
    }
    //directional light
    if(0)
    {
        setVec3(&c->cube_shader,"dir_light.ambient", light.ambient);
        setVec3(&c->cube_shader,"dir_light.diffuse", light.diffuse);
        setVec3(&c->cube_shader,"dir_light.specular", light.specular);
        setVec3(&c->cube_shader,"dir_light.direction", normalize_vec3({1.f,-1.f,0.f}));
    }
    //point light
    {
       setVec3(&c->cube_shader,"point_lights[0].ambient", light.ambient);
       setVec3(&c->cube_shader,"point_lights[0].diffuse", light.diffuse);
       setVec3(&c->cube_shader,"point_lights[0].specular", light.specular);
       glUniform3f(glGetUniformLocation(c->cube_shader.ID, "point_lights[0].position"), light_pos.x, light_pos.y, light_pos.z); 
       setFloat(&c->cube_shader,"point_lights[0].constant",  1.0f);
       setFloat(&c->cube_shader, "point_lights[0].linear",    0.022f);
       setFloat(&c->cube_shader, "point_lights[0].quadratic", 0.0019f); 
    }
    if(0)
    {
        setVec3(&c->cube_shader,"point_lights[1].ambient", light.ambient);
        setVec3(&c->cube_shader,"point_lights[1].diffuse", light.diffuse);
        setVec3(&c->cube_shader,"point_lights[1].specular", light.specular);
        glUniform3f(glGetUniformLocation(c->cube_shader.ID, "point_lights[1].position"), 0.f, 0.f, 0.f); 
       setFloat(&c->cube_shader,"point_lights[1].constant",  1.0f);
       setFloat(&c->cube_shader, "point_lights[1].linear",    0.022f);
       setFloat(&c->cube_shader, "point_lights[1].quadratic", 0.0019f); 
    }




    glUniform3f(glGetUniformLocation(c->cube_shader.ID, "view_pos"), camera_pos.x, camera_pos.y, camera_pos.z); 

    setInt(&c->cube_shader, "point_light_count", 1); 
    setInt(&c->cube_shader, "dir_light_count", 1); 
    glBindVertexArray(c->vao);
    glDrawArrays(GL_TRIANGLES,0, 36);
    glBindVertexArray(0);
}



//DEPRECATED.. JUST FOR TESTING!!
static void 
init_cube(Cube* c)
{
    shader_load(&c->cube_shader,"../assets/shaders/phong.vert", "../assets/shaders/phong.frag");
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
        
    u32 vbo;
    glGenVertexArrays(1, &c->vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(c->vao);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute    //NOTE(ilias):maybe update face normals after transform??
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

static void 
render_cube(Cube* c, mat4* projection, mat4* view, vec3 light_pos, vec3 camera_pos)
{
    use_shader(&c->cube_shader);
    Light light = {light_pos,{0.2f, 0.2f, 0.2f},{0.7f, 0.7f, 0.7f},{1.0f, 1.0f, 1.0f}};
    mat4 model = translate_mat4(c->center);
    model.elements[0][0] =0.1f;
    model.elements[1][1] =0.1f;
    model.elements[2][2] =0.1f;
    setMat4fv(&c->cube_shader, "proj", (GLfloat*)projection->elements);
    setMat4fv(&c->cube_shader, "view", (GLfloat*)view->elements);
    setMat4fv(&c->cube_shader, "model", (GLfloat*)model.elements);
    {
        setVec3(&c->cube_shader,"m.ambient", {0.2f, 0.2f, 0.2f});
        setVec3(&c->cube_shader,"m.diffuse", {0.7f, 0.3f, 0.2f});
        setVec3(&c->cube_shader,"m.specular", {0.1f, 0.1f, 0.1f});
        setFloat(&c->cube_shader, "m.shininess", 3.f);
    }
    {
        setVec3(&c->cube_shader,"light.ambient", light.ambient);
        setVec3(&c->cube_shader,"light.diffuse", light.diffuse);
        setVec3(&c->cube_shader,"light.specular", light.specular);
        glUniform3f(glGetUniformLocation(c->cube_shader.ID, "light.position"), light_pos.x, light_pos.y, light_pos.z); 
    }

    glUniform3f(glGetUniformLocation(c->cube_shader.ID, "view_pos"), camera_pos.x, camera_pos.y, camera_pos.z); 

    glBindVertexArray(c->vao);
    glDrawArrays(GL_TRIANGLES,0, 36);
    glBindVertexArray(0);
}


