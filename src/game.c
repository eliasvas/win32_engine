#include "platform.h"
#include "ext/HandmadeMath.h" #include "tools.h"
#include "audio.h"
#include "entity.h"
#include "shader.h"
#include "shadowmap.h"
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
#include "collada_parser.h"
#include <string>  //just for to_string

static Camera cam;
static BitmapFont bmf;
static Sprite s;
static Cube c;
static Terrain terrain;

static PositionManager position_manager;
static Physics2DManager physics2d_manager;

static Model m;
static Model cubes[9];
static Model teapot_model;
static Model test_model;

static mat4 view_matrix;
static mat4 perspective_matrix;
static mat4 ortho_matrix;
static Renderer rend;
static DirLight dir_light;
static PointLight point_light;
static Skybox skybox;


b32 debug_menu = 1;
f32 inverted = 0.f;

#define SOUND_ON 0
#define COLLIDERS_ON 1
#define SKYBOX_ON 1

static AnimatedModel skel;
static MeshData data; //this should be an AnimatedModel
static Animation animation_to_play;
static Animator animator;


static Entity braid_guy;

void init(void)
{
    play_sound();
    data = read_collada(str(&global_platform.permanent_storage,"../assets/sat.dae"));
    animation_to_play = read_collada_animation(str(&global_platform.permanent_storage,"../assets/sat.dae"));

    init_fake_framebuffer();
    init_text(&bmf,"../assets/ASCII_512.png"); 
    
    init_camera(&cam);
    init_renderer(&rend);

    init_terrain(&terrain,"../assets/noise.png");
    //initializing all component managers
    {
        init_physics2d_manager(&physics2d_manager);
    }
    //entity init
    {
        braid_guy = create_entity();
        PhysicsBody2DComponent *c = add_component(&physics2d_manager, braid_guy);
        c->pos = v2(5,0);
        c->collider.box = aabb2d(s.box.min, s.box.min + s.scale);
        c->collider.type = BOX2D;
    }
    {
        load_model_data(m.vertices, "../assets/cube.obj", "../assets/basic.mtl");
        init_model(&m, m.vertices);
        m.spec_name = "white.png";
        m.diff_name = "green.png";
        m.scale = v3(1,1,1);
        for (i32 i = 0; i < 9; ++i)
        {
            cubes[i] = m;
            cubes[i].position = v3(i%3 + 5, 0, (i) / 3);
        }
    }
    //teapot_model initialization
    {
        load_model_dataOG(teapot_model.vertices, "../assets/utah_teapot.obj", "../assets/basic.mtl");
        init_model(&teapot_model, teapot_model.vertices);
        teapot_model.diff_name = "red.png";
        teapot_model.scale = {0.02,0.02,0.02};

    }
    //AnimatedModel initialization
    {
        Texture* diff = find_texture(&rend.manager, "red.png");
        skel = init_animated_model(diff,data.root,&data);
        //animation_to_play = read_animation_from_collada(..);
        animator = {skel, &animation_to_play, 0.3f};
    }
    //test model
    {
        load_model_data(test_model.vertices, "../assets/utah_teapot.obj", "../assets/basic.mtl");
        init_model(&test_model, data.verts, data.vertex_count);
        test_model.diff_name = "red.png";
        test_model.position = {0,2.f,0.0};
        test_model.scale = {0.2,0.2,0.2};
        test_model.rotation = quat_from_angle(v3(1,0,0), -PI/2);

    }

    //player initializiation
    {

        AnimationInfo info; 
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
     point_light = {{0,1,1},1.f,0.022f,0.0019f,{0.2f, 0.2f, 0.2f},{0.7f, 0.7f, 0.7f},{1.0f, 1.0f, 1.0f}};

}



void update(void)
{

    dir_light = {normalize_vec3(v3(0.2f,-0.7f,0.2f)),{0.4f, 0.4f, 0.4f},{0.7f, 0.7f, 0.7f},{0.1,0.1,0.1}};
    //if key K is pressed take a screenshot
    if (global_platform.key_pressed[KEY_K])
    {
        f32 *pixels = (f32*)malloc(sizeof(f32) * 3 * global_platform.window_width* global_platform.window_height); 
        glReadPixels(0, 0, global_platform.window_width,global_platform.window_height,GL_RGB, GL_FLOAT, pixels);
        ppm_save_current_framebuffer( global_platform.window_width, global_platform.window_height, pixels);
        free(pixels);
    }

    if (global_platform.key_pressed[KEY_L])
       play_sound(); 


    m.position = {5.f,0,0};
    teapot_model.position = {6, abs(sin(global_platform.current_time)/4.f) + 2.f,sin(global_platform.current_time) * 1.5f};
    test_model.position = {0, 1.f,0.0};
    change_to_fake_framebuffer();

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

    point_light.position = v3(sin(global_platform.current_time)* 3, 8,10);
    //point_light.position = {0, 8,3};

    view_matrix = get_view_mat(&cam);
    perspective_matrix = perspective_proj(45.f,global_platform.window_width / (float)global_platform.window_height, 0.1f,100.f); 
    ortho_matrix = orthographic_proj(-6.f,6.f,-6.f,6.f, 0.1, 100.f);


#if COLLIDERS_ON 
    //--------YES---------
    renderer_push_AABB(&rend, {v3(-1,0,1),v3(1, 5, 2.5)}); //eyeballed for animated model
    renderer_push_rect(&rend,{s.box.min, s.scale.y,s.scale.x});
    
    //---------NO---------
    //renderer_push_rect(&rend,{v2(3,3), 1,1});
    //renderer_push_rect(&rend,{v2(2,2), 2,4});
    //renderer_push_rect(&rend,{v2(1,1), 2,1});
#endif
    renderer_push_dir_light(&rend,&dir_light);
    //renderer_push_point_light_info(&rend,point_light.position,point_light.ambient , point_light.diffuse, point_light.specular);
    renderer_push_mesh(&rend,&teapot_model, teapot_model.vertices.size());
    //renderer_push_mesh(&rend,&m, m.vertices.size());
    for (i32 i = 0; i < 9; ++i)
        renderer_push_mesh(&rend,&cubes[i], cubes[i].vertices.size());
    renderer_set_projection_matrix(&rend, perspective_matrix);
    renderer_set_view_matrix(&rend, view_matrix);
    renderer_set_camera_pos(&rend, cam.pos);
    mat4 ortho = orthographic_proj(-10.f,10.f,-10.f,10.f,0.1f,9.f); //we use orthographic projection because we do direction lights..
    renderer_set_ortho_matrix(&rend, ortho);
#if COLLIDERS_ON
    render_colliders(&physics2d_manager,&rend);
#endif

}

void render(void)
{

#if SKYBOX_ON
    render_skybox(&skybox,perspective_matrix, view_matrix);
#endif


    render_sprite(&s, &rend);  //we probably dont need this
    //renderer_push_rect(&rend,{s.pos , s.scale.y,s.scale.x});

    mat4 mat = mul_mat4(perspective_matrix, view_matrix);

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

    //update should be at update(void) but whatever
    update_animator(&animator);
    render_animated_model(&animator.model, &rend.shaders[2], rend.perspective_projection, rend.view_matrix);

    render_to_framebuffer0(inverted);


}
