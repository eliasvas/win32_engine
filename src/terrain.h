#ifndef TERRAIN_H
#define TERRAIN_H


#include "tools.h"
#include "shader.h"
#include "texture.h"
#include "model.h" //for vertex struct definition only!!

#define VERTEX_COUNT 64 
#define MAX_HEIGHT 20
#define MIN_HEIGHT 0
#define MAX_WIDTH 20
#define MIN_WIDTH 0



struct Terrain
{
    GLuint vao;
    Texture tex;
    Shader shader;
    Image image;
    mat4 model;
};

static f32 
get_height_from_heightmap(Terrain* terrain, u32 x, u32 z)
{
    int dx = (terrain->image.width*x)/VERTEX_COUNT;
    int dz = (terrain->image.height*z*4)/VERTEX_COUNT;
    return ((float)terrain->image.data[(dx + dz * VERTEX_COUNT)*4]) / 255.f;
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
            vertices[vp*3] = ( (f32)j / (f32)(VERTEX_COUNT - 1) ) * MAX_WIDTH; 
            vertices[vp*3 + 1] = get_height_from_heightmap(terrain, i, j) * 2; 
            vertices[vp*3 + 2] = ( (f32)i / (f32)(VERTEX_COUNT - 1) ) * MAX_HEIGHT; 
            normals[vp*3] = 0;
            normals[vp*3 + 1] = 1;
            normals[vp*3 + 2] = 0;
            tex_coords[vp*2] = ( (f32)j / (f32)(VERTEX_COUNT - 1) );
            tex_coords[vp*2 + 1] = ( (f32)i / (f32)(VERTEX_COUNT - 1) );
            vp++;
        }
    }
    i32 indices[6*(VERTEX_COUNT-1)*(VERTEX_COUNT-1)]; //the indices of all the triangles that should be drawn
    vertex pack[VERTEX_COUNT -1]; //here are packs of {pos, normal, texcoord}
    //float data[24] = {0.0,0.0,0.0,   0.0,0.0,1.0,   1.0,0.0,0.0,  0.0,0.0,1.0,  0.0,0.0,1.0,   0.0,0.0,1.0,    1.0,0.0,1.0,    0.0,0.0,1.0};


    //we fill the element buffer

    int index = 0;
    for (i32 z = 0; z <VERTEX_COUNT-1; ++z)
    {
        for (i32 x = 0; x < VERTEX_COUNT-1; ++x)
        {
            i32 topleft = (VERTEX_COUNT*z) + x;
            i32 topright = topleft + 1;
            i32 bottomleft = ((z + 1)*VERTEX_COUNT) + x;
            i32 bottomright = bottomleft + 1;

            indices[index++] = topleft;
            indices[index++] = bottomleft;
            indices[index++] = topright;
            indices[index++] = topright;
            indices[index++] = bottomleft;
            indices[index++] = bottomright;
        }
    }
     

    GLuint vbo1, vbo2, ebo;
    glGenVertexArrays(1, &terrain->vao);
    glGenBuffers(1, &vbo1);
    glGenBuffers(1, &vbo2);
    glGenBuffers(1, &ebo);
    glBindVertexArray(terrain->vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE, 0* sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

}

static void
render_terrain(Terrain *terrain, mat4 proj, mat4 view)
{
    use_shader(&terrain->shader);
    glBindVertexArray(terrain->vao);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, terrain->tex.id);
    mat4 MVP = mul_mat4(proj, view);
    setMat4fv(&terrain->shader, "MVP", (GLfloat*)MVP.elements);
    //glDrawElements(GL_LINES, (VERTEX_COUNT-1) * (VERTEX_COUNT-1)*6, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINES, (VERTEX_COUNT-1) * (VERTEX_COUNT-1)*6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}
#endif
