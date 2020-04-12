#include "ext/HandmadeMath.h"
#include "help.h"

#include "shader.h"
#include "platform.h"
#include "texture.h"
#include "camera.h"
#include "quad.h"
#include "cube.h"
#include "text.h"
#include "renderer.h"
#include <string>  //just for to_string

/*
TODO(ilias):Things that need to be done 

@Play Music 
	-initialize dsound
	-load wavs

@Dedicated 2D renderer

@Make the ECS (+Sprites)

@Particles!

@Physics Engine

@Platforming

@Animations!

*/
hmm_mat4 MVP;
static shader s;
static shader basic;
static shader cube_shader;
static f32 global_counter;
static camera cam;
static bitmap_font bmf;
static quad q;
static quad q2;
static cube c;
static hmm_mat4 view_matrix;
static hmm_mat4 projection_matrix;
static renderer rend;
b32 debug_menu = 1;
void load_shaders(){
    shader_load(&s,"../assets/shaders/textured_quad.vert", "../assets/shaders/textured_quad.frag");
    shader_load(&basic,"../assets/shaders/basic.vert", "../assets/shaders/basic.frag");
    shader_load(&cube_shader,"../assets/shaders/colored_cube.vert", "../assets/shaders/colored_cube.frag");
}
void init()
{
    init_text(&bmf,"../assets/ASCII_512.png"); 
    global_counter = 0.f;
    load_shaders();
    init_camera(&cam);
    init_cube(&c);
    init_quad(&q, "../assets/verybadguy.png");
    init_quad(&q2, "../assets/arrow.png");
    q2.pos = {0.f,1.f,0.f};
    c.center = {0,0,0};
    init_renderer(&rend);
}

void update(platform* p)
{
    renderer_begin(&rend, p->window_width, p->window_height);
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
    if (p->key_pressed[KEY_TAB] != debug_menu)
    {
       debug_menu = !debug_menu; 
    }
    view_matrix = get_view_mat(&cam);
    //projection_matrix =HMM_Orthographic(-10, 10, -10, 10, 90,300);
    projection_matrix = HMM_Perspective(HMM_ToRadians(45.f),800.f/600.f, 0.1f,200.f); 

    hmm_vec2 rect_pos = {1,0.5};
    hmm_vec2 rect_scale = {1,1};
    hmm_vec2 rect_pos2 = {0.6,0.6};
    //renderer_update(&rend);
    renderer_push(&rend,-0.5);
    renderer_push(&rend, 0.5);
    //renderer_push_rect(&rend, rect_pos, rect_scale);
}

void render(HDC *DC, platform* p)
{
    //renderer_render(&rend);
    renderer_render(&rend);
    if (debug_menu){ 
        //TODO(ilias): do this with homemade C impl
        print_text(&bmf,"#console#",0,570, 20);
        std::string g_t = std::to_string(p->current_time);
        g_t.resize(5);
        std::string t("time: " +g_t) ;
        print_text(&bmf,t.c_str(),0,540, 20);
        std::string w = std::to_string(p->window_width);
        std::string h = std::to_string(p->window_height);
        std::string t2("wsize: " + w + "x"+ h) ;
        print_text(&bmf,t2.c_str(),0,510, 20); 

    }

    SwapBuffers(*DC);
    return;
    glClearColor(1.f - global_counter,0.1f,0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//drawing quad
    glUseProgram(s.ID);
    {
        hmm_mat4 model_matrix = HMM_Translate(q.pos);
        MVP = HMM_MultiplyMat4(projection_matrix,HMM_MultiplyMat4(view_matrix, model_matrix));
    }
    setMat4fv(&s, "MVP", (float*) MVP.Elements);
    glBindVertexArray(q.VAO);
    //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    {
        hmm_mat4 model_matrix = HMM_Translate(c.center);
        hmm_mat4 scale_matrix = HMM_Scale({0.5f,0.5f,0.5f});
        model_matrix = HMM_MultiplyMat4(model_matrix,scale_matrix);
        MVP = HMM_MultiplyMat4(view_matrix, model_matrix);
        MVP = HMM_MultiplyMat4(projection_matrix,MVP);
        //use_shader(&q.s);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, q2.t.id);
        //setInt(&q.s, "ourTexture",1);

        render_quad(&q, (float*)MVP.Elements);
    }

    glUseProgram(cube_shader.ID);
    {
        hmm_mat4 model_matrix = HMM_Translate(c.center);
        //hmm_mat4 scale_matrix = HMM_Scale({0.5,0.5,0.5});
        //hmm_mat4 scale_matrix = HMM_Scale({0.5,0.5,0.5});
        //model_matrix = HMM_MultiplyMat4(model_matrix, scale_matrix);
        hmm_mat4 model_rotation = HMM_Rotate(global_counter * 180 / 78, {0.f,1.f,0.f});
        model_matrix = HMM_MultiplyMat4(model_matrix,model_rotation);
        MVP = HMM_MultiplyMat4(view_matrix, model_matrix); //NOTE(ilias):maybe change the direction of multiplication
        MVP = HMM_MultiplyMat4(projection_matrix,MVP);
    }
    setMat4fv(&cube_shader, "MVP", (float*)MVP.Elements);
    glBindVertexArray(c.VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 36); //NOTE(ilias): uncomment to draw cube
    glBindVertexArray(0);


    if (debug_menu){ 
        //TODO(ilias): do this with homemade C impl
        print_text(&bmf,"#console#",0,570, 20);
        std::string g_t = std::to_string(p->current_time);
        g_t.resize(5);
        std::string t("time: " +g_t) ;
        print_text(&bmf,t.c_str(),0,540, 20);
        std::string w = std::to_string(p->window_width);
        std::string h = std::to_string(p->window_height);
        std::string t2("wsize: " + w + "x"+ h) ;
        print_text(&bmf,t2.c_str(),0,510, 20); 

    }
    SwapBuffers(*DC);

}
