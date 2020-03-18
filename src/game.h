#include "ext/HandmadeMath.h"
#include "help.h"
#include "shader.h"
#include "platform.h"

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

static f32 vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};  
static u32 VBO,VAO;
static shader s;

static f32 global_counter;
void init()
{
    //init_shaders():
    global_counter = 0.f;

    //we load the shader
    shader_load(&s, "../assets/shaders/basic.vert", "../assets/shaders/basic.frag");
    //we make the triangle
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
}

void update(platform* p)
{
    f32 dt = p->dt;
    global_counter += (f32)0.001;
    if (p->key_down[KEY_A])
    {
        global_counter = 0.0f;
    }
}

void render(HDC *DC)
{
    glClearColor(1.f - global_counter,0.2f,0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(s.ID);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SwapBuffers(*DC);

}
