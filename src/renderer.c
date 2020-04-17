#include "renderer.h"

void* get_array_buffer_ptr()
{
    return glMapBuffer(GL_ARRAY_BUFFER,GL_MAP_READ_BIT);
}

static void 
init_renderer(renderer* rend)
{
    rend->tex_count = 0;
    GLfloat vertices[8] = {
                            0.0f,0.0f,
                            1.0f,0.0f,
                            0.0f,1.0f,
                            1.0f,1.0f,
                            };



    {//initializing sprite VAO
        glGenVertexArrays(1, &rend->sprite_vao);    
        glBindVertexArray(rend->sprite_vao);
        glGenBuffers(1, &rend->sprite_instance_vbo);
        glGenBuffers(1,&rend->vertex_vbo);

        //binding per-vertex vbo
        glBindBuffer(GL_ARRAY_BUFFER, rend->vertex_vbo);
        glBufferData(GL_ARRAY_BUFFER, 8*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
        //vertex positions (per-vertex)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2* sizeof(GLfloat), (GLvoid*)0);
        //vertex texture-coordinates (per-vertex)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 2*sizeof(GLfloat), (GLvoid*)(0));



        //binding per-instance vbo
        glBindBuffer(GL_ARRAY_BUFFER,rend->sprite_instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rend->sprite_instance_data),  rend->sprite_instance_data,GL_DYNAMIC_DRAW);
        //position (per-instance)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sprite), (GLvoid*)0);
        glVertexAttribDivisor(2, 1);
        //scale (per-instance)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(sprite), (GLvoid*)(2* sizeof(GLfloat)));
        glVertexAttribDivisor(3, 1);
        //texture unit (per-instance)
        glEnableVertexAttribArray(4);
        glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(sprite), (GLvoid*)(4 * sizeof(GLfloat)));
        glVertexAttribDivisor(4, 1);
        //opacity (per-instance)
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(sprite), (GLvoid*)(5 * sizeof(GLfloat)));
        glVertexAttribDivisor(5, 1);



        glBindVertexArray(0);
    }

    //this is for testing purposes.. make it normal.. god

    shader_load(&rend->shaders[0], "../assets/shaders/batch.vert", "../assets/shaders/batch.frag");

    load_texture(&rend->tex[0],"../assets/verybadguy.png");
    rend->tex_count++;
    load_texture(&rend->tex[1],"../assets/notsobadguy.png");
    rend->tex_count++;
}

static void
renderer_render(renderer* rend,float* proj, GLfloat time)
{
    v2 texture_sizes[TEXTURE_MAX];

    glBindVertexArray(rend->sprite_vao);
    //update the isntanced array
    glBindBuffer(GL_ARRAY_BUFFER, rend->sprite_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rend->sprite_instance_data), rend->sprite_instance_data, GL_STATIC_DRAW);


    for (i32 i = 0; i< rend->tex_count;++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, rend->tex[i].id);
        texture_sizes[i] = {(GLfloat)rend->tex[i].width, (GLfloat)rend->tex[i].height};
    }

    use_shader(&rend->shaders[0]);
    setMat4fv(&rend->shaders[0],"projection_matrix",proj);

    //passing the available tex_units as uniform
    GLuint loc = glGetUniformLocation(rend->shaders[0].ID, "slots");
    GLint arr[2] = {0,1};
    glUniform1iv(loc, 2,arr);

    //passing the dimensions of the available tex_units as uniform
    loc = glGetUniformLocation(rend->shaders[0].ID, "tex_sizes");
    glUniform2fv(loc, TEXTURE_MAX, (GLfloat*)texture_sizes); 

    loc = glGetUniformLocation(rend->shaders[0].ID, "iTime");
    glUniform1f(loc, time);


    //glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rend->sprite_alloc_pos); // 10 diamonds, 4 vertices per instance
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rend->sprite_alloc_pos); // 10 diamonds, 4 vertices per instance
    glBindVertexArray(0);
}

static void
renderer_begin(renderer* rend, i32 w, i32 h)
{
    rend->render_width = w;
    rend->render_height = h;
    rend->rect_alloc_pos = 0;
    rend->sprite_alloc_pos = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1);
    glViewport(0, 0, (GLsizei)rend->render_width, (GLsizei)(rend->render_height)); //for some reason this is the only viewport called
    //glViewport(0, 0, (GLsizei)rend->render_width, (GLsizei)(rend->render_height*(w/(float)h))); //this is the holy saviour???
    //NOTE(ilias): projection matrix should be provided at initialization..
    rend->projection_matrix = HMM_Perspective(HMM_ToRadians(45.f),800.f/600.f, 0.1f,200.f); //TODO(ilias): change to HMM_Orthographic!! 
}

static void
renderer_push(renderer* rend, v2 offset, GLuint unit)
{

    sprite to_add = {offset,{1,1}, unit,1.0f};
    //rend->sprite_instance_data[rend->sprite_alloc_pos/sizeof(sprite)] = to_add; //NOTE(ilias): maybe memcpy
    rend->sprite_instance_data[rend->sprite_alloc_pos] = to_add; //NOTE(ilias): maybe memcpy
    rend->sprite_alloc_pos++; 
}



