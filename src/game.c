#include "ext/HandmadeMath.h"
#include "tb.h"

#include "shader.h"
#include "platform.h"
#include "texture.h"
#include "camera.h"
#include "quad.h"
#include "cube.h"
#include "text.h"
#include "renderer.h"
#include "animation.h"
#include "model.h"
#include <string>  //just for to_string

/*
TODO(ilias):Things that need to be done 

@Play Music 
	-initialize dsound
	-load wavs
@Particles!

@Physics Engine

@Platforming
*/
mat4 MVP;
static camera cam;
static bitmap_font bmf;
static sprite s;
static model m;
static mat4 view_matrix;
static mat4 projection_matrix;
static renderer rend;
b32 debug_menu = 1;
void init(void)
{
    init_text(&bmf,"../assets/ASCII_512.png"); 
    init_camera(&cam);

    {
        load_model_data(m.vertices, "../assets/utah_teapot.obj", "../assets/utah_teapot.mtl");
        init_model(&m, m.vertices);
    }

    {
        animation_info info; 
        init_animation_info(&info,{0.0f,0.0f}, {1.f/6.f, 1.0f}, 2, 6, 0.1f, 0);
        init_sprite(&s, {-2.5,0.0},{0.5,1.25}, 2, 1.f, info);
    }

    init_renderer(&rend);
}

void update(void)
{
    global_platform.vsync = 1;
    renderer_begin(&rend, global_platform.window_width, global_platform.window_height);
    update(&cam);
    update_animation_info(&s.info);

    if (global_platform.key_pressed[KEY_TAB] != debug_menu)
    {
       debug_menu = !debug_menu; 
    }

    //player movement
    {
        if(global_platform.key_pressed[KEY_RIGHT])
        {
            s.pos = add_vec2(s.pos,mul_vec2f({1.0,0.0}, global_platform.dt));
            s.flip = 0;
        }else if (global_platform.key_pressed[KEY_LEFT])
        {
            s.pos = sub_vec2(s.pos,mul_vec2f({1.0,0.0}, global_platform.dt));
            s.flip = 1;
        }
    }
    view_matrix = get_view_mat(&cam);
    projection_matrix = perspective_proj(45.f,global_platform.window_width / (float)global_platform.window_height, 0.1f,100.f); 

    int current_frame = ((int)global_platform.current_time) % 6; 
    renderer_push(&rend, {(GLfloat)2.f,(GLfloat)0.0f}, {1,1},(GLuint)0);
    renderer_push(&rend, s.pos,s.scale, s.texture_unit, (s.info).bottom_left, (s.info).dim, s.flip);
}

void render(HDC *DC)
{
    mat4 mat = mul_mat4(projection_matrix, view_matrix);
    renderer_render(&rend, (float*)mat.elements);

    if (debug_menu){ 
        print_text(&bmf,"|console|",0,570, 20);
        std::string g_t = std::to_string(global_platform.current_time);
        g_t.resize(5);
        std::string t("time: " +g_t);
        print_text(&bmf,t.c_str(),0,540, 20);
        std::string w = std::to_string(global_platform.window_width);
        std::string h = std::to_string(global_platform.window_height);
        std::string t2("wsize: " + w + "x"+ h) ;
        print_text(&bmf,t2.c_str(),0,510, 20); 
        std::string t3("ms: " + std::to_string(global_platform.dt));
        print_text(&bmf,t3.c_str(),0,490, 20);
    }
#if 0
    {
        vec3 rotation_axis = {0,1,0};
        rotation_axis = normalize_vec3(rotation_axis);
        mat4 rotation = rotate_mat4(abs(sin(global_platform.current_time)*180), rotation_axis);
        mat4 model_mat = translate_mat4({0,-5.5,-30.f -50}); //changing translate changes color???!
        model_mat = mul_mat4(projection_matrix,mul_mat4(model_mat, rotation));
        render_model(&m, model_mat);
    }
#endif
    {
        mat4 model_mat = translate_mat4({0,-5.5,-30.f -50.f}); //changing translate changes color???!
        model_mat = mul_mat4(mat,model_mat);
        render_model(&m, model_mat);
    }
        //model_mat2 = HMM_MultiplyMat4(projection_matrixh,model_mat2);



    SwapBuffers(*DC);

}
