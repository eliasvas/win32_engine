#include "ext/HandmadeMath.h"
float a = 0.f;
static const f32 MAX_DISTANCE = 5.f;
struct Camera
{
    vec3 pos;
    vec3 dir;
    vec3 front;
};

static void 
turn_camera_around_center(Camera* cam, f32 degrees)
{
}

static void 
init_camera (Camera* cam)
{
    cam->pos = {0.f,0.f,4.f};
    cam->front = {0.0f,0.0f,-1.0f};
}

static void 
move_camera_wrt_player(Camera* cam, vec2 player_pos)
{
    cam->pos.x  = player_pos.x;
    cam->pos.y = player_pos.y;
}

static void 
move_camera_wrt_player_smooth(Camera* cam, vec2 player_pos)
{
}


static void 
update(Camera* cam)
{
    //a+= 0.01;
    //if (cam->pos.Z < 1.f)cam->pos = {0.f,0.f,300.f};
    //cam->pos.Z -= 1.f;
    //cam->pos.Z++;
    if (global_platform.key_down[KEY_A])
    {
        cam->pos.x -= 0.1f;
    }
    if (global_platform.key_down[KEY_D])
    {
        cam->pos.x += 0.1f;
    }
    if (global_platform.key_down[KEY_W])
    {
        cam->pos.y += 0.1f;
    }
    if (global_platform.key_down[KEY_S])
    {
        cam->pos.y -= 0.1f;
    }
    if (global_platform.key_down[KEY_Q])
    {
        turn_camera_around_center(cam, 1.f);
    }
   if (global_platform.key_down[KEY_E])
    {
        turn_camera_around_center(cam, -1.f);
    }
    cam->dir = {0,0,-1}; //NOTE(ilias): maybe orientation is messed up?
    //cam->pos = {cam->pos[0] + cos(a),cam->pos[1] + sin(a)};
}
static void 
update_wrt_player(Camera* cam, vec2 player_pos)
{
    move_camera_wrt_player(cam, player_pos);
}


mat4 get_view_mat(Camera* cam)
{
    mat4 camera = look_at(cam->pos,add_vec3(cam->pos, {0.0,0.0,-1.0}),{0.f,1.f,0.f});
    //hmm_mat4 camera = HMM_LookAt(cam->pos, HMM_SubtractVec3({0.0,0.0,0.0}, cam->pos),{0.f,1.f,0.f});
    return camera;
}

