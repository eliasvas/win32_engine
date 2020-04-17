#include "ext/HandmadeMath.h"
float a = 0.f;
struct camera
{
    hmm_vec3 pos;
    hmm_vec3 dir;
    hmm_vec3 front;
};

void turn_camera_around_center(camera* cam, f32 degrees)
{
}

void init_camera (camera* cam)
{
    cam->pos = {0.f,0.f,400.f};
    //hmm_vec3 target = MM_LookAt(hmm_vec3 Eye, hmm_vec3 Center, hmm_vec3 Up);
    cam->front = {0.0f,0.0f,-1.0f};
}
//TODO(ilias): maybe put controls in camera??
void update(platform* p, camera* cam)
{
    //a+= 0.01;
    //if (cam->pos.Z < 1.f)cam->pos = {0.f,0.f,300.f};
    //cam->pos.Z -= 1.f;
    //cam->pos.Z++;
    if (p->key_down[KEY_A])
    {
        cam->pos.X -= 0.1f;
    }
    if (p->key_down[KEY_D])
    {
        cam->pos.X += 0.1f;
    }
    if (p->key_down[KEY_W])
    {
        cam->pos.Y += 0.1f;
    }
    if (p->key_down[KEY_S])
    {
        cam->pos.Y -= 0.1f;
    }
    if (p->key_down[KEY_Q])
    {
        turn_camera_around_center(cam, 1.f);
    }
   if (p->key_down[KEY_E])
    {
        turn_camera_around_center(cam, -1.f);
    }
    cam->dir = {0,0,-1}; //NOTE(ilias): maybe orientation is messed up?
    //cam->pos = {cam->pos[0] + cos(a),cam->pos[1] + sin(a)};
}

hmm_mat4 get_view_mat(camera* cam)
{
    hmm_mat4 camera = HMM_LookAt(cam->pos,HMM_AddVec3(cam->pos, cam->front),{0.f,1.f,0.f});
    return camera;
}

