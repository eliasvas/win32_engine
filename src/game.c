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
static shader basic;
static f32 global_counter;
static camera cam;
static bitmap_font bmf;
static cube c;
static cube c2;
static hmm_mat4 view_matrix;
static hmm_mat4 projection_matrix;
static renderer rend;
b32 debug_menu = 1;
void load_shaders(){
    shader_load(&basic,"../assets/shaders/basic.vert", "../assets/shaders/basic.frag");
}
void init()
{
    global_counter = 0.f;

    init_text(&bmf,"../assets/ASCII_512.png"); 
    load_shaders();
    init_camera(&cam);

    init_cube(&c);
    c.center = {0,0,0};

    init_cube(&c2);
    c2.center = {1.0,1.0,0.0};

    init_renderer(&rend);
}

void update(platform* p)
{
    p->vsync = 1;
    renderer_begin(&rend, p->window_width, p->window_height);
    global_counter += 0.1f;
    update(p,&cam);
    if (p->key_down[KEY_SPACE])
    {
        global_counter = 0.0f;
    }
    if (p->key_pressed[KEY_R])
    {
        reload_shader_from_files(&basic.ID,basic.vertex_str,basic.fragment_str);
    }
    if (p->key_pressed[KEY_TAB] != debug_menu)
    {
       debug_menu = !debug_menu; 
    }
    view_matrix = get_view_mat(&cam);
    //projection_matrix =HMM_Orthographic(-10, 10, -10, 10, 90,300);
    projection_matrix = HMM_Perspective(HMM_ToRadians(45.f),p->window_width / (float)p->window_height, 0.1f,500.f); 

    hmm_vec2 rect_pos = {1,0.5};
    hmm_vec2 rect_scale = {1,1};
    hmm_vec2 rect_pos2 = {0.6,0.6};
    for (GLfloat i = -1.0f; i < 1.0f;i+=0.5)
    {
        for (GLfloat j = -1.0f; j < 1.0f; j+=0.5)
        {
            //renderer_push(&rend, {(GLfloat)i,(GLfloat)j}, (GLuint)(0));
        }
    }
    //renderer_push_rect(&rend, rect_pos, rect_scale);
    //c.center.Z = 100* abs(sin(p->current_time));
}

void render(HDC *DC, platform* p)
{
    hmm_mat4 mat = HMM_Multiply(projection_matrix, view_matrix);//HMM_Translate({0.0,0.0,0.0});
    renderer_render(&rend, (float*)mat.Elements, p->current_time);
    //rendering the cube
    {
        hmm_vec3 rotation_axis = {p->dt*100,p->current_time};
        rotation_axis = HMM_Normalize(rotation_axis);
        hmm_mat4 rotation = HMM_Rotate(sin(p->current_time)*20, rotation_axis);
        hmm_mat4 cm = HMM_Multiply(mat,rotation);
        render_cube(&c, cm);
    }
    {
        hmm_vec3 rotation_axis = {p->dt*100,p->current_time};
        rotation_axis = HMM_Normalize(rotation_axis);
        hmm_mat4 rotation = HMM_Rotate(sin(p->current_time)*20, rotation_axis);
        hmm_mat4 trans = HMM_Translate(c2.center);
        hmm_mat4 cm = HMM_Multiply(mat,HMM_Multiply(trans, rotation));
        render_cube(&c2, cm);
    }

    if (debug_menu){ 
        print_text(&bmf,"|console|",0,570, 20);
        std::string g_t = std::to_string(p->current_time);
        g_t.resize(5);
        std::string t("time: " +g_t);
        print_text(&bmf,t.c_str(),0,540, 20);
        std::string w = std::to_string(p->window_width);
        std::string h = std::to_string(p->window_height);
        std::string t2("wsize: " + w + "x"+ h) ;
        print_text(&bmf,t2.c_str(),0,510, 20); 
        std::string t3("ms: " + std::to_string(p->dt));
        print_text(&bmf,t3.c_str(),0,490, 20);
    }
    SwapBuffers(*DC);

}
