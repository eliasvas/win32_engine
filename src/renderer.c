#include "renderer.h"

void* get_array_buffer_ptr()
{
    return glMapBuffer(GL_ARRAY_BUFFER,GL_MAP_READ_BIT);
}

static void 
init_renderer(renderer* rend)
{
    rend->tex_count = 0;
    GLfloat vertices[16] = {
                            -0.05f,-0.05f,0.0f,0.0f,
                            -0.05f,0.05f,1.0f,0.0f,
                            0.05f,-0.05f,0.0f,1.0f,
                            0.05f,0.05f,1.0f,1.0f
                            };



    {//initializing sprite VAO
        glGenVertexArrays(1, &rend->sprite_vao);    
        glBindVertexArray(rend->sprite_vao);
        glGenBuffers(1, &rend->sprite_instance_vbo);
        glGenBuffers(1,&rend->vertex_vbo);

        //binding per-vertex vbo
        glBindBuffer(GL_ARRAY_BUFFER, rend->vertex_vbo);
        glBufferData(GL_ARRAY_BUFFER, 16*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
        //vertex positions (per-vertex)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4* sizeof(GLfloat), (GLvoid*)0);
        //vertex texture-coordinates (per-vertex)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 4*sizeof(GLfloat), (GLvoid*)(2*sizeof(GLfloat)));



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
renderer_render(renderer* rend)
{
    glBindVertexArray(rend->sprite_vao);
    //update the isntanced array
    glBindBuffer(GL_ARRAY_BUFFER, rend->sprite_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rend->sprite_instance_data), rend->sprite_instance_data, GL_STATIC_DRAW);


    //GLubyte* ptr = (GLubyte*)get_array_buffer_ptr();
    //sprite arr[32];
    //memcpy(arr, ptr, sizeof(sprite));
    
    for (i32 i = 0; i< rend->tex_count;++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, rend->tex[i].id);
    }

    //render the damn thing
    use_shader(&rend->shaders[0]);

    GLuint loc = glGetUniformLocation(rend->shaders[0].ID, "slots[0]");
    glUniform1i(loc, 0);
    loc = glGetUniformLocation(rend->shaders[0].ID, "slots[1]");
    glUniform1i(loc,1);

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
    glViewport(0, 0, (GLsizei)rend->render_width, (GLsizei)rend->render_height);
    
    //NOTE(ilias): projection matrix should be provided at initialization..
    rend->projection_matrix = HMM_Perspective(HMM_ToRadians(45.f),800.f/600.f, 0.1f,200.f); //TODO(ilias): change to HMM_Orthographic!! 
}
static void
renderer_push(renderer* rend, v2 offset, GLuint unit)
{

    sprite to_add = {offset,{1,1}, unit};
    //rend->sprite_instance_data[rend->sprite_alloc_pos/sizeof(sprite)] = to_add; //NOTE(ilias): maybe memcpy
    rend->sprite_instance_data[rend->sprite_alloc_pos] = to_add; //NOTE(ilias): maybe memcpy
    rend->sprite_alloc_pos++; 
}



