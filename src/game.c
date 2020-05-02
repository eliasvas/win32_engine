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
#include "sprite.h"
#include "animation.h"
#include "model.h"
#include "physics.h"
#include "postproc.h"
#define CUTE_SOUND_IMPLEMENTATION
#include "ext/cute_sound.h"
#include <string>  //just for to_string

mat4 MVP;
static Camera cam;
static BitmapFont bmf;
static Sprite s;
static Sprite enemy;
static quad background;
static Model m;
static mat4 view_matrix;
static mat4 perspective_matrix;
static mat4 ortho_matrix;
static renderer rend;
b32 debug_menu = 1;
f32 inverted = 0.f;
static cs_context_t* ctx;

static cs_playing_sound_t sound;
static cs_loaded_sound_t loaded;
#define sound_on 0
#define colliders_on 1

static Box b1;
static Box b2;
static Box b3;
static Box b4;

/*GAMEPLPAY CODE*/
#include <vector>

#define MAX_ENEMIES 100
typedef Sprite Enemy;
Enemy enemies[MAX_ENEMIES];
u32 enemies_index = 0;

void init(void)
{

    init_fake_framebuffer();
    init_text(&bmf,"../assets/ASCII_512.png"); 

    init_camera(&cam);

    {
        load_model_data(m.vertices, "../assets/utah_teapot.obj", "../assets/utah_teapot.mtl");
        init_model(&m, m.vertices);
    }
    //player initializiation
    {

        AnimationInfo info; 
        //init_animation_info(AnimationInfo* info, vec2 bl, i32 frames_per_row, i32 frames_per_col, f32 tex_unit, i32 frame_count, f32 time_per_frame, b32 play_once)
        init_animation_info(&info,{0.0f,(1.f - 1.f/7.f)}, 7, 7, 4, 50, 0.05f, 0);
        init_sprite(&s, {-2.5,0.0},{1.f,1.3f}, 4, 1.f, info);
        //s.is_blinking = 1;
        s.box.hb = {{0.3f,0.0f}, 0.4f,0.6f};
    }
    //enemy initialization
    {
        Enemy enemy;
        AnimationInfo info;
        init_animation_info(&info, {0.0f,0.0f},{1.f, 1.0f},5,1,0.05f,0);
        //init_sprite(Sprite *s, vec2 pos, vec2 scale, GLuint tex_unit, GLfloat opacity, AnimationInfo info)
        init_sprite(&enemies[enemies_index++], {0.0f,1.0f},{1.0f,1.0f},5,1.f,info);
        enemies[enemies_index -1].box.hb = {{0.2,0.2},0.6,0.6};
    }
    //background initialization
    {
        init_quad(&background,"../assets/background.png");
        background.pos = {0,0,0};
    }
    //initializing collider boxes
    {
       init_Box(&b1,{-25.f,-25.f},1.f,50.f); 
       init_Box(&b2,{-25.f,-25.f},50.f,1.f); 
       init_Box(&b3,{-26.f,26.f},55.f,1.f); 
       init_Box(&b4,{26.f,-26.f},1.f,55.f); 
    }
    init_collider_render_quad();
    init_renderer(&rend);

#if sound_on
    ctx =cs_make_context(WND,44000,8192,0,0);
    loaded = cs_load_wav("../assets/background_music.wav");
    //cs_play_sound_def_t def = cs_make_def(&loaded);
    sound = cs_make_playing_sound(&loaded);
    cs_insert_sound(ctx, &sound);
    //cs_spawn_mix_thread(ctx);
#endif
}

void update(void)
{
    change_to_fake_framebuffer();
#if sound_on
    cs_mix(ctx);
#endif


    global_platform.vsync = 1;
    renderer_begin(&rend, global_platform.window_width, global_platform.window_height);
    update_wrt_player(&cam, {s.box.min.x, s.box.min.y});
    update_animation_info_plus_ultra(&s.info);
    update_animation_info(&enemy.info);

    if (global_platform.key_pressed[KEY_TAB] != debug_menu)
    {
       debug_menu = !debug_menu; 
    }

    if (global_platform.key_pressed[KEY_SPACE])
        inverted = 1.f;
    else
        inverted = 0.f;

    {
        if(global_platform.key_pressed[KEY_RIGHT])
        {
            if (s.box.velocity.x < 0.f)s.box.velocity.x += 6.f * global_platform.dt;
            s.box.velocity.x += 6.f* global_platform.dt; //constant must be speed
            s.flip = 0;
        }
        if (global_platform.key_pressed[KEY_LEFT])
        {
            if (s.box.velocity.x > 0.f)s.box.velocity.x -= 6.f * global_platform.dt;
            s.box.velocity.x -= 6.f * global_platform.dt;
            s.flip = 1;
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

    //enemies update
    {
        for (Enemy& e : enemies){
            vec2 target = add_vec2(s.box.min, {s.box.w/2.f, s.box.h/2.f}); 
            vec2 direction = sub_vec2(target, add_vec2(e.box.min, {e.box.w/2.f,e.box.h/2.f}));  
            //f32 a = (float)rand()/((float)RAND_MAX/45.f);
            //direction = rotate_vec2(direction, a);
            e.box.velocity = add_vec2(e.box.velocity,direction);
            e.box.velocity = mul_vec2f(e.box.velocity, 0.01f);
            e.box.velocity.x = max(-5, min(e.box.velocity.x, 5));
            e.box.velocity.y = max(-5, min(e.box.velocity.y, 5));
            e.box.w = min(abs(sin(global_platform.current_time))/3.f + 0.7f, 1.f);
            e.box.h = min(abs(sin(global_platform.current_time))/3.f + 0.7f, 1.f);

            e.box.min = add_vec2(e.box.min,e.box.velocity);
        }
    }

    //update collisions
    {
        for(u32 i = 0; i < colliders.size();++i)
        {
            colliders[i]->is_colliding = 0;
            //if (check_collision(*colliders[i]))colliders[i]->is_colliding = 1;
            for(u32 j = 0; j < colliders.size();++j)
            {
                if (collide(colliders[i], colliders[j])){
                    //handle_collision_basic(colliders[i], colliders[j]);
                    resolve_collision(colliders[i], colliders[j]);
                }
            }
        }
        
    }
    view_matrix = get_view_mat(&cam);
    perspective_matrix = perspective_proj(43.f,global_platform.window_width / (float)global_platform.window_height, 0.1f,100.f); 
    ortho_matrix = orthographic_proj(-6.f,6.f,-6.f,6.f, 0.1, 100.f);

    int current_frame = ((int)global_platform.current_time) % 6; 
    //renderer_push(&rend, {(GLfloat)2.f,(GLfloat)0.0f}, {1,1},(GLuint)0);
    render_sprite(&s, &rend);
    //render_sprite(&enemy, &rend);
    for (Enemy& e: enemies)
        render_sprite(&e, &rend);
}

void render(HDC *DC)
{
    mat4 mat = mul_mat4(ortho_matrix, view_matrix);
        //rendering background 
    {
        mat4 background_mat = mul_mat4(mat, translate_mat4({1.0,1.0,1.0}));
        render_quad(&background, (float*)background_mat.elements);
    }
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
        std::string t4("camera: {" + std::to_string(cam.pos.x) + ", " + std::to_string(cam.pos.y) + ", " + std::to_string(cam.pos.z) + "}");
        print_text(&bmf,t4.c_str(),0,470, 15);
        std::string t5 = ("Enemies spawned: " + enemies_index);
        print_text(&bmf,t5.c_str(),0,450, 15);
    }
    std::string g_t = std::to_string(global_platform.current_time);
    g_t.resize(4);
    print_text(&bmf,g_t.c_str(),790/2.f,570, 20);

#if 0
    {
        mat4 model_mat = translate_mat4({0,-5.5,-30.f -50.f}); //changing translate changes color???!
        model_mat = mul_mat4(mat,model_mat);
        render_model(&m, model_mat);
    }
        //model_mat2 = HMM_MultiplyMat4(perspective_matrixh,model_mat2);
#endif

#if colliders_on
    //NOTE(ilias): this is for drawing colliders!
    glDisable(GL_DEPTH_TEST); //NOTE(ilias): this is used only for collider visualization
    glLineWidth(2);
    //render_collider_in_pos (mat, {s.box.min.x, s.box.min.y,-1.f}, {s.box.w,s.box.h}, (float)s.box.is_colliding);
    //render_collider_in_pos (mat, {enemy.box.min.x, enemy.box.min.y,-1.f}, {enemy.box.w,enemy.box.h}, (float)enemy.box.is_colliding);
    //render_collider_in_pos (mat, {b1.min.x,b1.min.y,-1.f}, {b1.w, b1.h}, (float)b1.is_colliding);
    for (Box *b : colliders)
        render_collider_in_pos (mat, {b->min.x + b->hb.min.x,b->min.y + b->hb.min.y,-1.f}, {b->w * b->hb.w, b->h * b->hb.h}, (float)b->is_colliding);
    glLineWidth(1);
    glEnable(GL_DEPTH_TEST);
#endif

    render_to_framebuffer0(inverted);

    SwapBuffers(*DC);

}
