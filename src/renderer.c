#include "renderer.h"

static void 
init_renderer(renderer* rend)
{
/*
    // vertices
    GLfloat vertices[] = {
         0.0f,   0.5f,
         0.5f, -0.5f, 
        -0.5f, -0.5f,
         0.0f,  -.1f,
    };
*/
    GLfloat vertices[8]; //this is the rect's vertex position (if we take them as vec2's)
    vertices[0] = -0.05;     //TODO(ilias): maybe make these vec2 rather than just floats
    vertices[1] = -0.05;
    vertices[2] = -0.05;
    vertices[3] = 0.05;
    vertices[4] = 0.05;
    vertices[5] = -0.05;
    vertices[6] = 0.05;
    vertices[7] = 0.05;

    {//initializing rect VAO
        glGenVertexArrays(1, &rend->rect_vao);    
        glGenBuffers(1, &rend->rect_instance_vbo);
        glGenBuffers(1, &rend->vertex_vbo);

        glBindVertexArray(rend->rect_vao);

        glBindBuffer(GL_ARRAY_BUFFER, rend->vertex_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

        glBindBuffer(GL_ARRAY_BUFFER, rend->rect_instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rend->rect_instance_data), rend->rect_instance_data, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(rect), (GLvoid*)0);
        glVertexAttribDivisor(1, 1); // This sets the vertex attribute to instanced attribute.
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(rect), (GLvoid*)(2* sizeof(GLfloat)));
        glVertexAttribDivisor(2, 1); // This sets the vertex attribute to instanced attribute.
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(rect), (GLvoid*)(4 * sizeof(GLfloat)));
        glVertexAttribDivisor(3, 1); // This sets the vertex attribute to instanced attribute.

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }


    {//initializing sprite VAO
        glGenVertexArrays(1, &rend->sprite_vao);    
        glBindVertexArray(rend->sprite_vao);
        glGenBuffers(1, &rend->sprite_instance_vbo);

        glBindBuffer(GL_ARRAY_BUFFER, rend->vertex_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

        glBindBuffer(GL_ARRAY_BUFFER,rend->sprite_instance_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(rend->sprite_instance_data),  rend->sprite_instance_data,GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(1);
        //position
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(sprite), (GLvoid*)0);
        glVertexAttribDivisor(1, 1);
        //scale
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(sprite), (GLvoid*)(2* sizeof(GLfloat)));
        glVertexAttribDivisor(2, 1); // This sets the vertex attribute to instanced attribute.
        //texture unit
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, sizeof(sprite), (GLvoid*)(4 * sizeof(GLfloat)));
        glVertexAttribDivisor(3, 1); // This sets the vertex attribute to instanced attribute.


    }

    //this is for testing purposes.. make it normal.. god
    char *vert = "#version 330 core\nlayout(location = 0) in vec2 position;layout(location = 1) in vec2 wps;layout(location = 2) in vec2 scale;layout(location = 3) in float offset;out float red;void main(){gl_Position = vec4(position.x + offset, position.y, 0.0, 1.0);red = offset;}";
    char *frag = "#version 330 core\nlayout(location = 0) out vec4 color;in float red;void main(){color = vec4(abs(red), 0.0, 0.0, 1.0f);}  ";
    rend->shaders[0].ID = load_shader_from_strings(vert, frag);
}

static void renderer_update(renderer* rend)
{
    float translations[10];
    for (int i = 0; i < 10; ++i)
    {
        translations[i] = i *(rand() / (float)RAND_MAX)/10.f;
    }
    glBindBuffer(GL_ARRAY_BUFFER, rend->rect_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(translations), translations, GL_STATIC_DRAW);

}

static void
renderer_render(renderer* rend)
{
    //update the isntanced array
    glBindBuffer(GL_ARRAY_BUFFER, rend->rect_instance_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rend->rect_instance_data), rend->rect_instance_data, GL_STATIC_DRAW);

    //render the damn thing
    use_shader(&rend->shaders[0]);
    glBindVertexArray(rend->rect_vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rend->rect_alloc_pos/sizeof(rect)); // 10 diamonds, 4 vertices per instance
    glBindVertexArray(0);
}

static void
renderer_begin(renderer* rend, i32 w, i32 h)
{
    rend->render_width = w;
    rend->render_height = h;
    rend->rect_alloc_pos = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1);
    glViewport(0, 0, (GLsizei)rend->render_width, (GLsizei)rend->render_height);
    
    //NOTE(ilias): projection matrix should be provided at initialization..
    rend->projection_matrix = HMM_Perspective(HMM_ToRadians(45.f),800.f/600.f, 0.1f,200.f); //TODO(ilias): change to HMM_Orthographic!! 
}
static void
renderer_push(renderer* rend, f32 offset)
{
    v2 pos;
    v2 scale;
    f32 color;

    rect to_add = {{0.4,0},{1,1}, offset};
    rend->rect_instance_data[rend->rect_alloc_pos/sizeof(rect)] = to_add; //NOTE(ilias): maybe memcpy
    rend->rect_alloc_pos += sizeof(rect); 
}



