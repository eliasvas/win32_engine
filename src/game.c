#include "ext/HandmadeMath.h"
#include "tools.h"

#include "shader.h"
#include "platform.h"
#include "texture.h"
#include "camera.h"
#include "quad.h"
#include "cube.h"
#include "text.h"
#include "renderer.h"
#include "sprite.h"
#include "animation.h"
#include "skybox.h"
#include "model.h"
#include "physics.h"
#include "postproc.h"
#include "terrain.h"
#include "shadowmap.h"
#define CUTE_SOUND_IMPLEMENTATION
#include "ext/cute_sound.h"
#include <string>  //just for to_string

static Camera cam;
static BitmapFont bmf;
static Sprite s;
static Cube c;
static Terrain terrain;

static Model m;
static Model teapot_model;

static mat4 view_matrix;
static mat4 perspective_matrix;
static mat4 ortho_matrix;
static Renderer rend;
static DirLight dir_light;
static PointLight point_light;
static Skybox skybox;


b32 debug_menu = 1;
f32 inverted = 0.f;

static cs_context_t* ctx;
static cs_playing_sound_t sound;
static cs_loaded_sound_t loaded;

#define sound_on 0
#define colliders_on 1
#define skybox_on 0

void init(void)
{
    init_fake_framebuffer();
    init_text(&bmf,"../assets/ASCII_512.png"); 

    init_camera(&cam);
    init_renderer(&rend);

    init_terrain(&terrain,"../assets/test.png");
    terrain.model = translate_mat4(v3(-8,0,-16));
    {
        load_model_data(m.vertices, "../assets/huge_plane.obj", "../assets/basic.mtl");
        init_model(&m, m.vertices);
        m.spec_name = "noise.png";
        //m.position = {0,-5,0};
        //m.scale = v3(0.4,0.1,1);
    }
    //teapot_model initialization
    {
        load_model_data(teapot_model.vertices, "../assets/utah_teapot.obj", "../assets/basic.mtl");
        init_model(&teapot_model, teapot_model.vertices);
        teapot_model.diff_name = "red.png";
        teapot_model.position = {0,2.f,0.0};
        teapot_model.scale = {0.02,0.02,0.02};

    }

    //player initializiation
    {

        AnimationInfo info; 
        //init_animation_info(AnimationInfo* info, vec2 bl, i32 frames_per_row, i32 frames_per_col, f32 tex_unit, i32 frame_count, f32 time_per_frame, b32 play_once)
        init_animation_info(&info,{0.0f,1.f - 1.f/4.f}, 7, 4, 3, 27, 0.025f, 0);
        //init_animation_info(&info,{0.0f,0.0f}, 1, 1, 1, 1, 300000.f, 0);
        init_sprite(&s, {-2.5,0.0},{1.f,1.3f}, 3, 1.f, info);
        //s.is_blinking = 1;
        s.box.hb = {{0.3f,0.0f}, 0.4f,0.6f};
    }
    //init the skybox
    {
        std::vector<std::string> faces;
        faces.push_back("../assets/nebula/purplenebula_rt.tga");
        faces.push_back("../assets/nebula/purplenebula_lf.tga");
        faces.push_back("../assets/nebula/purplenebula_up.tga");
        faces.push_back("../assets/nebula/purplenebula_dn.tga");
        faces.push_back("../assets/nebula/purplenebula_bk.tga");
        faces.push_back("../assets/nebula/purplenebula_ft.tga");
        init_skybox(&skybox, faces);
    }
    //init lights
    {
         point_light = {{0,1,1},1.f,0.022f,0.0019f,{0.2f, 0.2f, 0.2f},{0.7f, 0.7f, 0.7f},{1.0f, 1.0f, 1.0f}};

         dir_light = {{0,-1,0},{0.2f, 0.2f, 0.2f},{0.7f, 0.7f, 0.7f},{0.3,0.3,0.3}};

    }

#if sound_on
    ctx =cs_make_context(WND,44000,8192,0,0);
    loaded = cs_load_wav("../assets/background_music.wav");
    sound = cs_make_playing_sound(&loaded);
    cs_insert_sound(ctx, &sound);
#endif
}

void update(void)
{
    teapot_model.position = {0, abs(sin(global_platform.current_time)/4.f) + 2.f,0.0};
    change_to_fake_framebuffer();
#if sound_on
    cs_mix(ctx);
#endif

    global_platform.vsync = 1;
    renderer_begin(&rend);
    update_cam(&cam);
    update_animation_info(&s.info);

    if (global_platform.key_pressed[KEY_TAB] != debug_menu)
    {
       debug_menu = !debug_menu; 
    }



    if (global_platform.key_pressed[KEY_P])
        inverted = 1.f;
    else
        inverted = 0.f;

    {
      if(global_platform.key_pressed[KEY_RIGHT])
        {
            s.flip = 0;
            if (s.box.velocity.x < 0.f)s.box.velocity.x += 6.f * global_platform.dt;
            s.box.velocity.x += 6.f* global_platform.dt; //constant must be speed
        }
        if (global_platform.key_pressed[KEY_LEFT])
        {
            s.flip = 1;
            if (s.box.velocity.x > 0.f)s.box.velocity.x -= 6.f * global_platform.dt;
            s.box.velocity.x -= 6.f * global_platform.dt;
        }
        if (global_platform.key_pressed[KEY_UP])
        {
            if (s.box.velocity.y < 0.f)s.box.velocity.y += 6.f * global_platform.dt;
            s.box.velocity.y += 6.f * global_platform.dt;
        }
        if (global_platform.key_pressed[KEY_DOWN])
        {
            if (s.box.velocity.y > 0.f)s.box.velocity.y -= 6.f * global_platform.dt;
            s.box.velocity.y -= 6.f * global_platform.dt;
        }
        s.box.velocity.x = max(-5, min(s.box.velocity.x, 5));
        s.box.velocity.y = max(-5, min(s.box.velocity.y, 5));
        //s.box.velocity = normalize_vec2(s.box.velocity);

        if (abs(s.box.velocity.x) > 0.01f && !global_platform.key_pressed[KEY_LEFT] && !global_platform.key_pressed[KEY_RIGHT])
            s.box.velocity.x += 0.2 * (-1.f)* (s.box.velocity.x / abs(s.box.velocity.x));
       if (abs(s.box.velocity.y) > 0.01f && !global_platform.key_pressed[KEY_DOWN] && !global_platform.key_pressed[KEY_UP])
            s.box.velocity.y += 0.2 * (-1.f)* (s.box.velocity.y / abs(s.box.velocity.y));

        s.box.min = add_vec2(s.box.min,mul_vec2f(s.box.velocity, global_platform.dt));
    }

    teapot_model.rotation = quat_from_angle(v3(0,1,0), 2 * PI *( sin(global_platform.current_time)));

    point_light.position = {sin(global_platform.current_time)* 3, 8,10};
    //point_light.position = {0, 8,3};

    view_matrix = get_view_mat(&cam);
    perspective_matrix = perspective_proj(45.f,global_platform.window_width / (float)global_platform.window_height, 0.1f,100.f); 
    ortho_matrix = orthographic_proj(-6.f,6.f,-6.f,6.f, 0.1, 100.f);

    renderer_push_dir_light(&rend,&dir_light);
    renderer_push_point_light_info(&rend,point_light.position,point_light.ambient , point_light.diffuse, point_light.specular);
    renderer_push_mesh(&rend,&teapot_model, teapot_model.vertices.size());
    renderer_push_mesh(&rend,&m, m.vertices.size());
    renderer_set_projection_matrix(&rend, perspective_matrix);
    renderer_set_view_matrix(&rend, view_matrix);
    renderer_set_camera_pos(&rend, cam.pos);
    mat4 ortho = orthographic_proj(-10.f,10.f,-10.f,10.f,0.1f,9.f); //we use orthographic projection because we do direction lights..
    renderer_set_ortho_matrix(&rend, ortho);
}

void render(void)
{
    render_sprite(&s, &rend);  //<----------------------------this is for the braid guy
#if skybox_on
    render_skybox(&skybox,perspective_matrix, view_matrix);
#endif

    mat4 mat = mul_mat4(perspective_matrix, view_matrix);
    //rendering background 
    {
        //render_quad(&background, mat);
    }
    renderer_render(&rend);
    
    //render_terrain(&terrain, perspective_matrix, view_matrix);

    if (debug_menu){ 
        print_text(&bmf,"|console|",0,570, 20);
        std::string g_t = std::to_string(global_platform.current_time);
        g_t.resize(5);
        std::string t("time: " +g_t);
        print_text(&bmf,t.c_str(),0,540, 20);
        std::string w = std::to_string(global_platform.window_width);
        std::string h = std::to_string(global_platform.window_height);
        std::string window_info("wsize: " + w + "x"+ h) ;
        print_text(&bmf,window_info.c_str(),0,510, 20);
        std::string fps_info("ms: " + std::to_string(global_platform.dt));
        print_text(&bmf,fps_info.c_str(),0,490, 20);
        std::string camera_info("camera: {" + std::to_string(cam.pos.x) + ", " + std::to_string(cam.pos.y) + ", " + std::to_string(cam.pos.z) + "}");
        print_text(&bmf,camera_info.c_str(),0,470, 15);
        std::string mouse_info("mouse: {" + std::to_string(global_platform.mouse_x) + ", " + std::to_string(global_platform.mouse_y) + "}" + " [" + std::to_string(global_platform.left_mouse_down) + "]" + " [" + std::to_string(global_platform.right_mouse_down) + "]");
        print_text(&bmf,mouse_info.c_str(),0,450, 15);

    }


    {
        //render_cube_textured(&c,&perspective_matrix, &view_matrix, point_light.position, cam.pos);
    }


#if colliders_on
    //NOTE(ilias): this is for drawing colliders!
    glDisable(GL_DEPTH_TEST); //NOTE(ilias): this is used only for collider visualization
    glLineWidth(2);
    for (Box *b : colliders)
        render_collider_in_pos (mat, v3(b->min.x + b->hb.min.x,b->min.y + b->hb.min.y,-1.f), v2(b->w * b->hb.w, b->h * b->hb.h), (float)b->is_colliding);
    glLineWidth(1);
    glEnable(GL_DEPTH_TEST);
#endif


    render_to_framebuffer0(inverted);


}
