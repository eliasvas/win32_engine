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
static Sprite king;
static quad background;
static Model m;
static mat4 view_matrix;
static mat4 perspective_matrix;
static mat4 ortho_matrix;
static renderer rend;
b32 debug_menu = 1;
f32 inverted = 0.f;
static cs_context_t* ctx;

static b32 DEATH = 0;
static float time_of_death = 0.f;
static float wait_of_death = 3.f;

static cs_playing_sound_t sound;
static cs_playing_sound_t sound2;
static cs_loaded_sound_t loaded;
static cs_loaded_sound_t destroy_corona;
static int king_hp =2;
static f32 king_wait = 2.f;
#define sound_on 1
#define colliders_on 0

static Box b1;
static Box b2;
static Box b3;
static Box b4;

Box hit_colliders[4];

/*GAMEPLPAY CODE*/
#include <vector>

#define MAX_ENEMIES 100
typedef Sprite Enemy;
Enemy enemies[MAX_ENEMIES];
u32 enemies_index = 0;

static void 
update_king(Sprite* king)
{
    if (king_hp < 0){DEATH = 1;if(time_of_death == 0.f)time_of_death = global_platform.current_time;}

    if (king->is_blinking)king_wait-=global_platform.dt;
    if (king_wait < 0.f){king_wait = 2.f;king->is_blinking = 0;} 
}
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
        init_animation_info(&info,{0.0f,(1.f - 1.f/7.f)}, 7, 7, 7, 50, 0.005f, 0);
        //init_animation_info(&info,{0.0f,0.0f}, 1, 1, 1, 1, 300000.f, 0);
        init_sprite(&s, {-2.5,0.0},{1.f,1.3f}, 7, 1.f, info);
        //s.is_blinking = 1;
        s.box.hb = {{0.3f,0.0f}, 0.4f,0.6f};
        s.box.id = 1024;
    }
    //enemy initialization
    {
        Enemy enemy;
        AnimationInfo info;
        init_animation_info(&info, {0.0f,0.0f},{1.f, 1.0f},5,1,0.01f,0);
        //init_sprite(Sprite *s, vec2 pos, vec2 scale, GLuint tex_unit, GLfloat opacity, AnimationInfo info)
        init_sprite(&enemies[enemies_index++], {0.0f,1.0f},{1.0f,1.0f},5,1.f,info);
        enemies[enemies_index -1].box.hb = {{0.2,0.2},0.6,0.6};
        enemies[enemies_index -1].box.id = 666;
    }
  //king initialization
    {
        AnimationInfo info;
        init_animation_info(&info, {1.0f,1.0f},{1.f, 1.0f},0,1,0.01f,0);
        //init_sprite(Sprite *s, vec2 pos, vec2 scale, GLuint tex_unit, GLfloat opacity, AnimationInfo info)
        init_sprite(&king, {0.0f,9.0f},{2.0f,2.0f},0,1.f,info);
        king.box.hb = {{0.6,0.2},0.6,0.6};
        king.box.id = 667;
        king.box.mass = 20.f;
    }

    //background initialization
    {
        init_quad(&background,"../assets/background.png");
        background.pos = {0,0,0};
    }
    //initializing collider boxes
    {
       init_Box(&b1,{-25.f,-25.f},1.f,50.f); 
       b1.id = -1;
       init_Box(&b2,{-25.f,-25.f},50.f,1.f); 
       b2.id = -1;
       init_Box(&b3,{-26.f,26.f},55.f,1.f); 
       b3.id = -1;
       init_Box(&b4,{26.f,-26.f},1.f,55.f); 
       b4.id = -1;

    }

    //init hit colliders
    {
        init_Box(&hit_colliders[0],{-1, 0},0.1f,1.f); 
        hit_colliders[0].id = 1;
        init_Box(&hit_colliders[1],{1, 0},0.1f,1.f); 
        hit_colliders[1].id = 1;
        init_Box(&hit_colliders[2],{0, -1},1.f,0.1f); 
        hit_colliders[2].id = 1;
        init_Box(&hit_colliders[3],{0, 1},1.f,0.1f); 
        hit_colliders[3].id = 1;
    }

    init_collider_render_quad();
    init_renderer(&rend);

#if sound_on
    ctx =cs_make_context(WND,44000,8192,0,0);
    loaded = cs_load_wav("../assets/background_music.wav");
    destroy_corona = cs_load_wav("../assets/explosion.wav");
    //cs_play_sound_def_t def = cs_make_def(&loaded);
    sound = cs_make_playing_sound(&loaded);
    sound2 = cs_make_playing_sound(&destroy_corona);
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

    if(DEATH)
    {
        wait_of_death -= global_platform.dt;
        if (wait_of_death < 0)
        {
            exit(666);
        }
        return;
    }


    global_platform.vsync = 1;
    renderer_begin(&rend, global_platform.window_width, global_platform.window_height);
    update_wrt_player(&cam, {s.box.min.x, s.box.min.y});
    update_animation_info_plus_ultra(&s.info);
    update_animation_info(&enemy.info);
    update_king(&king);

    if (global_platform.key_pressed[KEY_TAB] != debug_menu)
    {
       debug_menu = !debug_menu; 
    }

    if (global_platform.key_pressed[KEY_D])
    {
        DEATH = 1;
        if(time_of_death == 0.f)time_of_death = global_platform.current_time;
    }


    if (global_platform.key_pressed[KEY_SPACE])
        inverted = 1.f;
    else
        inverted = 0.f;

    {
        if(global_platform.key_pressed[KEY_RIGHT])
        {
            s.texture_unit = 7; 
            s.info.bottom_leftOG = {0, 1 - 1/7.f};
            s.flip = 0;
            s.info.dim = {1/7.f,1/7.f};
            if (s.box.velocity.x < 0.f)s.box.velocity.x += 6.f * global_platform.dt;
            s.box.velocity.x += 6.f* global_platform.dt; //constant must be speed
        }
        if (global_platform.key_pressed[KEY_LEFT])
        {
                s.texture_unit = 8; 
                s.flip = 1;
            s.info.bottom_leftOG = {0, 1 - 1/7.f};
                s.info.dim = {1/7.f,1/7.f};
            if (s.box.velocity.x > 0.f)s.box.velocity.x -= 6.f * global_platform.dt;
            s.box.velocity.x -= 6.f * global_platform.dt;
        }
        if (global_platform.key_pressed[KEY_UP])
        {
            if (!global_platform.key_pressed[KEY_LEFT] && !global_platform.key_pressed[KEY_RIGHT]){
                s.texture_unit = 6; 
                s.info.bottom_leftOG = {0, 1 - 1/7.f};
                s.info.dim = {1/7.f,1/7.f};
                //s.flip = 1;
            }
            if (s.box.velocity.y < 0.f)s.box.velocity.y += 6.f * global_platform.dt;
            s.box.velocity.y += 6.f * global_platform.dt;
        }
        if (global_platform.key_pressed[KEY_DOWN])
        {
            if (!global_platform.key_pressed[KEY_LEFT] && !global_platform.key_pressed[KEY_RIGHT]){
                s.texture_unit = 9; 
                s.info.bottom_leftOG = {0, 1.f - 1/7.f};
                s.info.dim = {1/7.f,1/7.f};
            }
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

    //update hit colliders
    {
        vec2 player_pos = s.box.min;
        hit_colliders[0].min = add_vec2(player_pos, {-0.1f,0});
        hit_colliders[1].min = add_vec2(player_pos, {1.1f,0});
        hit_colliders[2].min = add_vec2(player_pos, {0,-0.2f});
        hit_colliders[3].min = add_vec2(player_pos, {0,1.1f});
    }
    //enemies update
    {
        for (Enemy& e : enemies){
            e.box.id = 666;
            if (!e.box.active)continue;
            vec2 target = add_vec2(king.box.min, {s.box.w/2.f, s.box.h/2.f}); 
            vec2 direction = sub_vec2(target, add_vec2(e.box.min, {e.box.w/2.f,e.box.h/2.f}));  
            e.box.velocity = add_vec2(e.box.velocity,direction);
            e.box.velocity = mul_vec2f(e.box.velocity, 0.01f);
            e.box.velocity.x = max(-5, min(e.box.velocity.x, 5));
            e.box.velocity.y = max(-5, min(e.box.velocity.y, 5));
            e.box.w = min(abs(sin(global_platform.current_time))/3.f + 0.7f, 1.f);
            e.box.h = min(abs(sin(global_platform.current_time))/3.f + 0.7f, 1.f);

            e.box.min = add_vec2(e.box.min,div_vec2f(e.box.velocity,2.f));
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
                collider_id = collide(colliders[i], colliders[j]);
                if (collider_id){
                    if (collider_id == 666){if (king.is_blinking == 0)king_hp--;king.is_blinking = 1;}
                    if (collider_id != 989) 
                        handle_collision_basic(colliders[i], colliders[j]);
                    else   
                        resolve_collision(colliders[i], colliders[j]);
                    if (collider_id == -989){colliders[j]->is_destroyed = 1;colliders[j]->active = 0;cs_insert_sound(ctx, &sound2);}
                }
            }
        }
        
    }
    //enemy_spawner
    {
        vec2 random_position = {(float)rand()/((float)RAND_MAX/25.f),   (float)rand()/((float)RAND_MAX/45.f)};
        if(enemies_index < 20 && (int)global_platform.current_time % 10 == 0){
            Enemy enemy;
            AnimationInfo info;
            init_animation_info(&info, {0.0f,0.0f},{1.f, 1.0f},5,1,0.05f,0);
            enemy.box.id = 666;
            //init_sprite(Sprite *s, vec2 pos, vec2 scale, GLuint tex_unit, GLfloat opacity, AnimationInfo info)
            init_sprite(&enemies[enemies_index++], {random_position},{1.0f,1.0f},5,1.f,info);
            enemies[enemies_index -1].box.hb = {{0.2,0.2},0.6,0.6};
        }
        for (int i = 0; i < 20 ;++i)
        {
            if (!enemies[i].box.active)
            {
                vec2 random_position = {(float)rand()/((float)RAND_MAX/25.f),   (float)rand()/((float)RAND_MAX/45.f)};
                enemies[i].box.min = random_position;
                enemies[i].box.active = 1;
            }
        }
    }

    view_matrix = get_view_mat(&cam);
    perspective_matrix = perspective_proj(43.f,global_platform.window_width / (float)global_platform.window_height, 0.1f,100.f); 
    ortho_matrix = orthographic_proj(-6.f,6.f,-6.f,6.f, 0.1, 100.f);

    int current_frame = ((int)global_platform.current_time) % 6; 
    //renderer_push(&rend, {(GLfloat)2.f,(GLfloat)0.0f}, {1,1},(GLuint)0);
    render_sprite(&s, &rend);
    render_sprite(&king, &rend);
    //render_sprite(&enemy, &rend);
    for (Enemy& e: enemies)
        if (e.box.active)render_sprite(&e, &rend);
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

    if (DEATH){
        std::string g_t = std::to_string(time_of_death);
        g_t.resize(5);
        std::string t("YOU SURVIVED: ");
        print_text(&bmf,t.c_str(), 70.f,570/1.5f, 50,  max(0.f,min(1.0,wait_of_death-1.f)));
        std::string t2(g_t + "SECONDS!");
        print_text(&bmf,t2.c_str(), 70.f,570/1.5f - 50.f, 50, max(0.f,min(1.0,wait_of_death-1.f)));
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
