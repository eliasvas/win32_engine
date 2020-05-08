#include "ext/HandmadeMath.h"
float a = 0.f;
static const f32 MAX_DISTANCE = 5.f;
struct Camera
{
    vec3 pos;
    vec3 front;
    vec3 up;
    f32 camera_speed = 2.f;
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
    cam->up = {0.f,1.f,0.f};
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
update_cam(Camera* cam)
{
    if (global_platform.key_down[KEY_A])
    {
        //cam->pos.x -= 0.1f; //we must make a right vector and move along it
        cam->pos = sub_vec3(cam->pos, mul_vec3f(cross_vec3(cam->front, cam->up), cam->camera_speed * global_platform.dt));
    }
    if (global_platform.key_down[KEY_D])
    {
        //cam->pos.x += 0.1f; //we must make a right vector and move along it
        cam->pos = add_vec3(cam->pos, mul_vec3f(cross_vec3(cam->front, cam->up), cam->camera_speed * global_platform.dt));
    }
    if (global_platform.key_down[KEY_W])
    {
        //cam->pos.y += 0.1f;
        cam->pos = add_vec3(cam->pos, mul_vec3f(cam->front,cam->camera_speed* global_platform.dt));
    }
    if (global_platform.key_down[KEY_S])
    {
        //cam->pos.y -= 0.1f;
        cam->pos = sub_vec3(cam->pos, mul_vec3f(cam->front,cam->camera_speed* global_platform.dt));
    }
    if (global_platform.key_down[KEY_Q])
    {
        turn_camera_around_center(cam, 1.f);
    }
   if (global_platform.key_down[KEY_E])
    {
        turn_camera_around_center(cam, -1.f);
    }
}
static void 
update_wrt_player(Camera* cam, vec2 player_pos)
{
    move_camera_wrt_player(cam, player_pos);
}


mat4 get_view_mat(Camera* cam)
{
    mat4 camera = look_at(cam->pos,add_vec3(cam->pos, cam->front),cam->up);
    //hmm_mat4 camera = HMM_LookAt(cam->pos, HMM_SubtractVec3({0.0,0.0,0.0}, cam->pos),{0.f,1.f,0.f});
#if 0
    const float radius = 10.0f;
    float camX = sin(global_platform.current_time) * radius;
    float camY = sin(global_platform.current_time) * radius;
    float camZ = cam->pos.z;
    camera = look_at(v3(camX, 0.0, camZ), v3(0.0, 0.0, 0.0), v3(0.0, 1.0, 0.0));
#endif
    return camera;
}

