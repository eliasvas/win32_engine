#include "ext/HandmadeMath.h"
#include "help.h"
#include "shader.h"
#include "platform.h"
#include "texture.h"
#include "camera.h"

/*
TODO(ilias):Things that need to be done 

@Play Music 
	-initialize dsound
	-load wavs

@load OBJ

@Text Rendering
	-stb_truetype?

@Make the ECS

@Particles!



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
hmm_mat4 MVP;
static u32 VBO,VAO, EBO;
static shader s;
static texture tex;
static f32 global_counter;
static camera cam;
void load_shaders(){
    shader_load(&s,"../assets/shaders/textured_quad.vert", "../assets/shaders/textured_quad.frag");
}
void init()
{
    global_counter = 0.f;
    load_shaders();
    init_camera(&cam);

    { //NOTE(ilias) should be moved in "quad(?)" struct??
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
        load_texture(&tex,"../assets/link.png");
    }
}

void update(platform* p)
{
    global_counter += 0.1f;
    update(&cam);
    if (p->key_down[KEY_SPACE])
    {
        global_counter = 0.0f;
    }
    if (p->key_pressed[KEY_R])
    {
        reload_shader_from_files(&s.ID,s.vertex_str,s.fragment_str);
    }
    hmm_mat4 model_matrix = HMM_Mat4d(1.f);
    hmm_mat4 view_matrix = HMM_Translate({0.f,0.f,-300.f});
    hmm_mat4 view_rotation = HMM_Rotate(global_counter * 180 / 2, {0.f,1.f,0.f});
    model_matrix = HMM_MultiplyMat4(model_matrix,view_rotation);
    view_matrix = HMM_LookAt(cam.pos,cam.dir,{0.f,1.f,0.f});
    hmm_mat4 projection_matrix = HMM_Perspective(HMM_ToRadians(45.0),800.f/600.f, -1.f, 50.f); 
    MVP = HMM_MultiplyMat4(view_matrix, model_matrix); //NOTE(ilias):maybe change the direction of multiplication
    MVP = HMM_MultiplyMat4(projection_matrix,MVP);

}

void render(HDC *DC)
{
    glClearColor(1.f - global_counter,0.2f,0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, tex.id);
    //setInt(&s, "ourTexture", 0);
    glUseProgram(s.ID);
    setMat4fv(&s, "MVP", (float*)MVP.Elements);
    glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SwapBuffers(*DC);

}
