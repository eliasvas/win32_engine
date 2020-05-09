#include "tb.h"

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
    return camera;
}
 

/* in case there is no math lib
INLINE mat4 look_at(vec3 eye, vec3 center, vec3 fake_up)
{
    mat4 res = m4();

    vec3 f = normalize_vec3(sub_vec3(center, eye));
    vec3 r = normalize_vec3(cross_vec3(f, fake_up));
    vec3 up = cross_vec3(r, f);

    res.elements[0][0] = r.x;
    res.elements[0][1] = up.x;
    res.elements[0][2] = -f.x;
    res.elements[0][3] = 0.0f;

    res.elements[1][0] = r.y;
    res.elements[1][1] = up.y;
    res.elements[1][2] = -f.y;
    res.elements[1][3] = 0.0f;

    res.elements[2][0] = r.z;
    res.elements[2][1] = up.z;
    res.elements[2][2] = -f.z;
    res.elements[2][3] = 0.0f;

    res.elements[3][0] = -dot_vec3(r, eye);
    res.elements[3][1] = -dot_vec3(up, eye);
    res.elements[3][2] = dot_vec3(f, eye);
    res.elements[3][3] = 1.0f;

    return res;
}
*/

