#include "renderer.h"

void* get_array_buffer_ptr()
{
    return glMapBuffer(GL_ARRAY_BUFFER,GL_MAP_READ_BIT);
}

static void 
init_renderer(renderer* rend)
{
    rend->tex_count = 0;
   
    {//initializing renderable VAO
        glGenVertexArrays(1, &rend->renderable_vao);    
        glBindVertexArray(rend->renderable_vao);
        glGenBuffers(1, &rend->renderable_instance_vbo);
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
        glBindBuffer(GL_ARRAY_BUFFER,rend->renderable_instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rend->renderable_instance_data),  rend->renderable_instance_data,GL_DYNAMIC_DRAW);
        //position (per-instance)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Renderable), (GLvoid*)0);
        glVertexAttribDivisor(2, 1);
        //scale (per-instance)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Renderable), (GLvoid*)(2* sizeof(GLfloat)));
        glVertexAttribDivisor(3, 1);
        //texture unit (per-instance)
        glEnableVertexAttribArray(4);
        glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Renderable), (GLvoid*)(4 * sizeof(GLfloat)));
        glVertexAttribDivisor(4, 1);
        //opacity (per-instance)
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Renderable), (GLvoid*)(5 * sizeof(GLfloat)));
        glVertexAttribDivisor(5, 1);
        //bottom_left coord
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Renderable), (GLvoid*)(6 * sizeof(GLfloat)));
        glVertexAttribDivisor(6, 1);
        //tex_dim
        glEnableVertexAttribArray(7);
        glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Renderable), (GLvoid*)(8 * sizeof(GLfloat)));
        glVertexAttribDivisor(7, 1);
        //flip
        glEnableVertexAttribArray(8);
        glVertexAttribIPointer(8, 1, GL_UNSIGNED_INT, sizeof(Renderable), (GLvoid*)(10 * sizeof(GLfloat)));
        glVertexAttribDivisor(8, 1);






        glBindVertexArray(0);
    }

    //this is for testing purposes.. make it normal.. god

    shader_load(&rend->shaders[0], "../assets/shaders/batch.vert", "../assets/shaders/batch.frag");

    //TODO make texture loading dynamic!!!!!!!!!! <---
    load_texture(&rend->tex[0],"../assets/KING.png");
    rend->tex_count++;
    load_texture(&rend->tex[1],"../assets/panda.png");
    rend->tex_count++;
    load_texture(&rend->tex[2],"../assets/runimation.png");
    rend->tex_count++;
    load_texture(&rend->tex[3],"../assets/bat.png");
    rend->tex_count++;
    load_texture(&rend->tex[4],"../assets/pandasheet.png");
    rend->tex_count++;
    load_texture(&rend->tex[5],"../assets/corona.png");
    rend->tex_count++;
   load_texture(&rend->tex[6],"../assets/pandabackrunsheet.png");
    rend->tex_count++;
   load_texture(&rend->tex[7],"../assets/pandaleftrunsheet.png");
    rend->tex_count++;
   load_texture(&rend->tex[8],"../assets/pandarightrunsheet.png");
    rend->tex_count++;
   load_texture(&rend->tex[9],"../assets/pandafrontrunsheet.png");
    rend->tex_count++;








}

static void
renderer_render(renderer* rend,float* proj)
{
    vec2 texture_sizes[TEXTURE_MAX];

    glBindVertexArray(rend->renderable_vao);
    //update the isntanced array
    glBindBuffer(GL_ARRAY_BUFFER, rend->renderable_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rend->renderable_instance_data), rend->renderable_instance_data, GL_STATIC_DRAW);


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
    GLint arr[10] = {0,1,2,3,4,5,6,7,8,9};
    glUniform1iv(loc, 10,arr);

    //passing the dimensions of the available tex_units as uniform
    loc = glGetUniformLocation(rend->shaders[0].ID, "tex_sizes");
    glUniform2fv(loc, TEXTURE_MAX, (GLfloat*)texture_sizes); 

    loc = glGetUniformLocation(rend->shaders[0].ID, "iTime");
    glUniform1f(loc, global_platform.current_time);


    //glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rend->renderable_alloc_pos); // 10 diamonds, 4 vertices per instance
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rend->renderable_alloc_pos); // 10 diamonds, 4 vertices per instance
    glBindVertexArray(0);
}

static void
renderer_begin(renderer* rend, i32 w, i32 h)
{
    rend->render_width = w;
    rend->render_height = h;
    rend->rect_alloc_pos = 0;
    rend->renderable_alloc_pos = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1);
    //glViewport(0, 0, (GLsizei)rend->render_width, (GLsizei)(rend->render_height)); //for some reason this is the only viewport called
    //glViewport(0, 0, (GLsizei)rend->render_width, (GLsizei)(rend->render_height*(w/(float)h))); //this is the holy saviour???
    //NOTE(ilias): projection matrix should be provided at initialization..
}

static void
renderer_push(renderer* rend, vec2 offset,vec2 scale, GLuint unit, GLuint flip = 0)
{

    Renderable to_add = {offset,scale, unit,1.0f,{0.0f,0.0f},{1.0f,1.0f}, flip};
    rend->renderable_instance_data[rend->renderable_alloc_pos] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_alloc_pos++; 
}


static void
renderer_push(renderer* rend, vec2 offset,vec2 scale, GLuint unit, vec2 bl, vec2 dim, GLuint flip = 0)
{
    Renderable to_add = {offset,scale, unit,1.0f,bl,dim, flip};
    //rend->renderable_instance_data[rend->renderable_alloc_pos/sizeof(Renderable)] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_instance_data[rend->renderable_alloc_pos] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_alloc_pos++; 
}

static void
renderer_push(renderer* rend, vec2 offset,vec2 scale, GLuint unit, vec2 bl, vec2 dim, GLuint flip, f32 opacity)
{
    Renderable to_add = {offset,scale, unit,opacity,bl,dim, flip};
    //rend->renderable_instance_data[rend->renderable_alloc_pos/sizeof(Renderable)] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_instance_data[rend->renderable_alloc_pos] = to_add; //NOTE(ilias): maybe memcpy
    rend->renderable_alloc_pos++; 
}


