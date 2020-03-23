#include "ext/HandmadeMath.h"
#include "help.h"
#include "shader.h"
#include "platform.h"
#include "texture.h"
#include "camera.h"
#include "quad.h"
#include "cube.h"

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
hmm_mat4 MVP;
static shader s;
static shader basic;
static shader cube_shader;
static f32 global_counter;
static camera cam;
static quad q;
static quad q2;
static cube c;
static hmm_mat4 view_matrix;
static hmm_mat4 projection_matrix;
void load_shaders(){
    shader_load(&s,"../assets/shaders/textured_quad.vert", "../assets/shaders/textured_quad.frag");
    shader_load(&basic,"../assets/shaders/basic.vert", "../assets/shaders/basic.frag");
    shader_load(&cube_shader,"../assets/shaders/colored_cube.vert", "../assets/shaders/colored_cube.frag");
}
void init()
{
    global_counter = 0.f;
    load_shaders();
    init_camera(&cam);
    init_cube(&c);
    init_quad(&q, "../assets/verybadguy.png");
    init_quad(&q2, "../assets/arrow.png");
    q2.pos = {0.f,1.f,0.f};
    c.center = {0,-1,0};
}

void update(platform* p)
{
    global_counter += 0.1f;
    update(p,&cam);
    if (p->key_down[KEY_SPACE])
    {
        global_counter = 0.0f;
    }
    if (p->key_pressed[KEY_R])
    {
        reload_shader_from_files(&s.ID,s.vertex_str,s.fragment_str);
    }
    view_matrix = HMM_LookAt(cam.pos,cam.dir,{0.f,1.f,0.f});
    projection_matrix = HMM_Perspective(HMM_ToRadians(45.f),800.f/600.f, 0.f,100.f); 
}

void render(HDC *DC)
{
    glClearColor(1.f - global_counter,0.1f,0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

//drawing quad
    texture * t = &q.t;
    glBindTexture(GL_TEXTURE_2D, t->id);
    glUseProgram(s.ID);
    {
        hmm_mat4 model_matrix = HMM_Translate(q.pos);
        hmm_mat4 model_rotation = HMM_Rotate(global_counter * 180 / 2, {0.f,1.f,0.f});
        model_matrix = HMM_MultiplyMat4(model_matrix,model_rotation);
        MVP = HMM_MultiplyMat4(view_matrix, model_matrix); //NOTE(ilias):maybe change the direction of multiplication
        MVP = HMM_MultiplyMat4(projection_matrix,MVP);
    }
    setInt(&s, "ourTexture", 0);
    setMat4fv(&s, "MVP", (float*)MVP.Elements);
    glBindVertexArray(q.VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    t = &q2.t;
    glBindTexture(GL_TEXTURE_2D, t->id);
    glUseProgram(s.ID);
    {
        hmm_mat4 model_matrix = HMM_Translate(q.pos);
        hmm_mat4 model_rotation = HMM_Rotate(global_counter * 180 / 2, {0.f,1.f,0.f});
        model_matrix = HMM_MultiplyMat4(model_matrix,model_rotation);
        MVP = HMM_MultiplyMat4(view_matrix, model_matrix); //NOTE(ilias):maybe change the direction of multiplication
        MVP = HMM_MultiplyMat4(projection_matrix,MVP);
    }
    setInt(&s, "ourTexture", 0);
    setMat4fv(&s, "MVP", (float*)MVP.Elements);
    glBindVertexArray(q2.VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


//drawing cube
    glUseProgram(cube_shader.ID);
    {
        hmm_mat4 model_matrix = HMM_Translate(c.center);
        hmm_mat4 model_rotation = HMM_Rotate(global_counter * 180 / 8, {0.4f,1.f,0.f});
        model_matrix = HMM_MultiplyMat4(model_matrix,model_rotation);
        MVP = HMM_MultiplyMat4(view_matrix, model_matrix); //NOTE(ilias):maybe change the direction of multiplication
        MVP = HMM_MultiplyMat4(projection_matrix,MVP);
    }
    setMat4fv(&cube_shader, "MVP", (float*)MVP.Elements);
    glBindVertexArray(c.VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
//  */
    SwapBuffers(*DC);

}
