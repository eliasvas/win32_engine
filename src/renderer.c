#include "renderer.h"

static void 
init_renderer(renderer* rend)
{
    GLint index = 0;
    for (GLint x = 0; x < 8; x++)
    {
        rend->translations[index++] = (GLfloat)x / 10.0f;
    }
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
    //GL

    // Setting VAOs and VBOs
    glGenVertexArrays(1, &rend->meshVAO);    
    glGenBuffers(1, &rend->instanceVBO);
    glGenBuffers(1, &rend->vertexVBO);

    glBindVertexArray(rend->meshVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rend->vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, rend->instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rend->translations), rend->translations, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), (GLvoid*)0);
    glVertexAttribDivisor(1, 1); // This sets the vertex attribute to instanced attribute.

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    char *vert = "#version 330 core\nlayout(location = 0) in vec2 position;layout(location = 1) in float offset;out float red;void main(){gl_Position = vec4(position.x + offset, position.y, 0.0, 1.0);red = 2*offset;}";
    char *frag = "#version 330 core\nlayout(location = 0) out vec4 color;in float red;void main(){color = vec4(abs(red), 0.0, 0.0, 1.0f);}  ";
    rend->s.ID = load_shader_from_strings (vert, frag);
}

static void renderer_update(renderer* rend)
{
    float translations[10];
    for (int i = 0; i < 10; ++i)
    {
        translations[i] = i *(rand() / (float)RAND_MAX)/10.f;
    }
    glBindBuffer(GL_ARRAY_BUFFER, rend->instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(translations), translations, GL_STATIC_DRAW);

}

static void
renderer_render(renderer* rend)
{
    //update the isntanced array
    glBindBuffer(GL_ARRAY_BUFFER, rend->instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rend->translations), rend->translations, GL_STATIC_DRAW);

    //render the damn thing
    use_shader(&rend->s);
    glBindVertexArray(rend->meshVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, rend->rect_alloc_pos/DIAMOND_SIZE); // 10 diamonds, 4 vertices per instance
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
    
    rend->projection_matrix = HMM_Perspective(HMM_ToRadians(45.f),800.f/600.f, 0.1f,200.f); //TODO(ilias): change to HMM_Orthographic!! 
}
static void
renderer_push(renderer* rend, f32 offset)
{
    rend->translations[rend->rect_alloc_pos/DIAMOND_SIZE] = offset;
    rend->rect_alloc_pos += DIAMOND_SIZE; 
}
/*
static void 
init_renderer(renderer* r)
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST); //NOTE(ilias): maybe have the depth test enabled

    r->rect_alloc_pos = 0;


    GLfloat verts[8]; //this is the rect's vertex position (if we take them as vec2's)
    verts[0] = -0.5;     //TODO(ilias): maybe make these vec2 rather than just floats
    verts[1] = -0.5;
    verts[2] = -0.5;
    verts[3] = 0.5;
    verts[4] = 0.5;
    verts[5] = -0.5;
    verts[6] = 0.5;
    verts[7] = 0.5;
    //GLfloat verts[8] = {0,0,0,1,1,0,1,1};

    //we initialize rect data!
    {
        glGenVertexArrays(1, &r->VAO);
        glBindVertexArray(r->VAO);
    
        glGenBuffers(1,&r->rect_buffer);
        glBindBuffer(GL_ARRAY_BUFFER,r->rect_buffer);
        glBufferData(GL_ARRAY_BUFFER,sizeof(verts), verts,GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,2,GL_FLOAT, GL_FALSE,0,0);

        // TODO(ilias): make a vertex buffer object to store the per-instance attributes!!
        glGenBuffers(1, &r->rect_instance_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, r->rect_instance_buffer);
        glBufferData(GL_ARRAY_BUFFER,sizeof(r->rects),0,GL_DYNAMIC_DRAW); //its sizeof(rects) because we want to be able
                                                                            //to use glBuffeSubData, for which we need a
                                                                            //complete array!!

        //position data
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, BYTES_PER_RECT, 0);
        glVertexAttribDivisor(1,1); //means that it increments per drawcall,
                                   //not for every call to the vertex shader!!!
        //size data
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, BYTES_PER_RECT, (void*)(sizeof(GL_FLOAT)* 2));
        glVertexAttribDivisor(2,1);


        glBindVertexArray(0);
    }
    
    //we initialize shaders!!
    {
        shader_load(&r->shaders[0],"../assets/shaders/batch.vert", "../assets/shaders/batch.frag"); //TODO(ilias): put actual instanced 2d shader
        //TODO(ilias): put more shaders!! (maybe particles/text ???)
    }
}

static void
renderer_begin(renderer* rend, f32 w, f32 h)
{ //TODO(ilias): make render_width and render_height scale according to platform layer from game.c side
    rend->render_width = w;
    rend->render_height = h;
    rend->rect_alloc_pos = 0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1);
    glViewport(0, 0, (GLsizei)rend->render_width, (GLsizei)rend->render_height);
    
    rend->projection_matrix = HMM_Perspective(HMM_ToRadians(45.f),800.f/600.f, 0.1f,200.f); //TODO(ilias): change to HMM_Orthographic!! 
}

static void 
renderer_push_rect(renderer* rend, hmm_vec2 pos, hmm_vec2 scale)
{
    //it puts the rect data in our dynamic alloc array
    hmm_vec4 poss = {0.1, pos.Y, scale.X,scale.Y};
    //memcpy(&rend->rects[0] + rend->rect_alloc_pos, &poss, sizeof(poss));
    *(rend->rects + rend->rect_alloc_pos) = {pos,scale}; //Note(ilias): HmmMMmMm  CHECK
    rend->rect_alloc_pos += BYTES_PER_RECT;
}

static void 
renderer_render(renderer* rend)
{
    //push data to the buffer
    i32 things_to_render = rend->rect_alloc_pos / BYTES_PER_RECT;//NOTE(ilias): if we have more than one thongs we must make another array
                                                                 //to hold such information..
    //puts rect buffer inside GPU memory
    glBindBuffer(GL_ARRAY_BUFFER, rend->rect_instance_buffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(rend->rects), rend->rects, GL_DYNAMIC_DRAW);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    use_shader(&rend->shaders[0]);
    
    glBindVertexArray(rend->VAO);
    {
        //setMat4(&rend->shader[0], "projection", rend->projection_matrix.Elements[0][0]);
        
        GLint first = 0;
        GLsizei count = 4;
        GLsizei instance_count = things_to_render;
        
        glDrawArraysInstanced(GL_TRIANGLE_STRIP,
                              first,
                              count,
                              instance_count);
    }
    glBindVertexArray(0);

    //glDrawArraysInstanced(GL_TRIANGLES, 0, 4, gl_InstanceID);
}

*/


