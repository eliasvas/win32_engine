#include "ext/HandmadeMath.h"
#include "help.h"
#include "shader.h"
#include "platform.h"
#include "texture.h"

/*
TODO(ilias):Things that need to be done 

@Play Music 
	-initialize dsound
	-load wavs

@Handle Input Universally

@load OBJ

@load Textures

@Text Rendering
	-stb_truetype?

@Make the ECS

@Particles!

@Camera


*/
static f32 quad_vertices[] = {
     // positions          // texture coords
     0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f    // top left
};


static u32 quad_indices[] = {  // note that we start from 0!
    0, 1, 3,  // first Triangle
    1, 2, 3   // second Triangle
};
static f32 tex_coords[]
{
   1.0f,1.0f, //top right corner
   0.0f,1.0f, //top left corner
   1.0f,0.0f, //bottom right corner
   0.0f,0.0f //bottom left corner 
};
static u32 VBO,VAO, EBO;
static shader s;
static texture tex;
static f32 global_counter;
void init()
{
    //init_shaders():
    global_counter = 0.f;

    //we load the shader
    shader_load(&s,"../assets/shaders/basic.vert", "../assets/shaders/basic.frag");
    //we make the triangle
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_indices), quad_indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE, 5* sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    load_texture(&tex,"../assets/tex.png");
}

void update(platform* p)
{
    global_counter += 0.1f;
    if (p->key_down[KEY_SPACE])
    {
        global_counter = 0.0f;
    }
    if (p->key_pressed[KEY_R])
    {
        reload_shader_from_files(&s.ID,s.vertex_str,s.fragment_str);
    }
}

void render(HDC *DC)
{
    glClearColor(1.f - global_counter,0.2f,0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, tex.id);
    glUseProgram(s.ID);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SwapBuffers(*DC);

}
