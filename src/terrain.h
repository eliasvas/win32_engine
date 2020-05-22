#pragma once
#include "tools.h"
#include "shader.h"
#include "texture.h"
#include "model.h" //for vertex struct definition only!!

#define VERTEX_COUNT 128
#define MAX_HEIGHT 40
#define MIN_HEIGHT 0
#define MAX_WIDTH 40
#define MIN_WIDTH 0



struct Terrain
{
    GLuint vao;
    Texture tex;
    Shader shader;
    Image image;
};

static f32 
get_height_from_heightmap(Terrain* terrain, u32 x, u32 z)
{
    return 0.0f;
}

static void
init_terrain(Terrain *terrain,const char *filename)
{
    load_texture(&terrain->tex,filename);
    shader_load(&terrain->shader, "../assets/shaders/mesh.vert", "../assets/shaders/mesh.frag");
    
    terrain->image = load_image_bytes(filename);

    i32 count = VERTEX_COUNT * VERTEX_COUNT;
    float vertices[VERTEX_COUNT * VERTEX_COUNT * 3];
    float normals[VERTEX_COUNT * VERTEX_COUNT * 3];
    float tex_coords[VERTEX_COUNT * VERTEX_COUNT * 2];

    //we fill the arrays holding each vertex, normal and texcoord (all parallel to each other)
    u32 vp = 0;
    for (u32 i = 0; i < VERTEX_COUNT; ++i)
    {
        for (u32 j = 0; j < VERTEX_COUNT; ++j)
        {
            vertices[vp*3] = ( (f32)j / (f32)(VERTEX_COUNT - 1) )* MAX_WIDTH; 
            vertices[vp*3 + 1] = 0; 
            vertices[vp*3 + 2] = ( (f32)i / (f32)(VERTEX_COUNT - 1) )* MAX_HEIGHT; 
            normals[vp*3] = 0;
            normals[vp*3 + 1] = 1;
            normals[vp*3 + 2] = 0;
            tex_coords[vp*2] = ( (f32)j / (f32)(VERTEX_COUNT - 1) );
            tex_coords[vp*2 + 1] = ( (f32)i / (f32)(VERTEX_COUNT - 1) );
        }
    }
    i32 indices[6*(VERTEX_COUNT-1)*(VERTEX_COUNT-1)]; //the indices of all the triangles that should be drawn
    vertex pack[VERTEX_COUNT -1]; //here are packs of {pos, normal, texcoord}
    float data[24] = {0.0,0.0,0.0,   0.0,0.0,1.0,   1.0,0.0,0.0,  0.0,0.0,1.0,  0.0,0.0,1.0,   0.0,0.0,1.0,    1.0,0.0,1.0,    0.0,0.0,1.0};


    //we fill the element buffer

    for (i32 z = 0; z <VERTEX_COUNT-1; ++z)
    {
        for (i32 x = 0; x < VERTEX_COUNT-1; ++x)
        {
            //vec3 bl =  {vertices[z * VERTEX_COUNT + x], vertices[z * VERTEX_COUNT + x + 1], vertices[z * VERTEX_COUNT + x + 2]};

        }
    }
     

    GLuint vbo, ebo;
    glGenVertexArrays(1, &terrain->vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(terrain->vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 6* sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

}

#if 0
static void
generateTerrain(){
    int count = VERTEX_COUNT * VERTEX_COUNT;
    float[] vertices = new float[count * 3];
    float[] normals = new float[count * 3];
    float[] textureCoords = new float[count*2];
    int[] indices = new int[6*(VERTEX_COUNT-1)*(VERTEX_COUNT-1)];
    int vertexPointer = 0;
    for(int i=0;i<VERTEX_COUNT;i++){
        for(int j=0;j<VERTEX_COUNT;j++){
            vertices[vertexPointer*3] = (float)j/((float)VERTEX_COUNT - 1) * MAX_HEIGHT;
            vertices[vertexPointer*3+1] = 0;
            vertices[vertexPointer*3+2] = (float)i/((float)VERTEX_COUNT - 1) * MAX_HEIGHT;
            normals[vertexPointer*3] = 0;
            normals[vertexPointer*3+1] = 1;
            normals[vertexPointer*3+2] = 0;
            textureCoords[vertexPointer*2] = (float)j/((float)VERTEX_COUNT - 1);
            textureCoords[vertexPointer*2+1] = (float)i/((float)VERTEX_COUNT - 1);
            vertexPointer++;
        }
    }
    int pointer = 0;
    for(int gz=0;gz<VERTEX_COUNT-1;gz++){
        for(int gx=0;gx<VERTEX_COUNT-1;gx++){
            int topLeft = (gz*VERTEX_COUNT)+gx;
            int topRight = topLeft + 1;
            int bottomLeft = ((gz+1)*VERTEX_COUNT)+gx;
            int bottomRight = bottomLeft + 1;
            indices[pointer++] = topLeft;
            indices[pointer++] = bottomLeft;
            indices[pointer++] = topRight;
            indices[pointer++] = topRight;
            indices[pointer++] = bottomLeft;
            indices[pointer++] = bottomRight;
        }
    }
}
#endif

static void
render_terrain(Terrain *terrain, mat4 proj, mat4 view)
{
    glBindVertexArray(terrain->vao);
    use_shader(&terrain->shader);
    mat4 MVP = mul_mat4(proj, view);
    setMat4fv(&terrain->shader, "MVP", (GLfloat*)MVP.elements);
    glDrawArrays(GL_TRIANGLE_STRIP, 0,VERTEX_COUNT - 1);
    glBindVertexArray(0);
}
