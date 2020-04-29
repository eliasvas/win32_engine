#pragma once
#include "tb.h"
#include "quad.h"
#define MAX_COLLIDERS 100
#include <vector>
using std::vector;

static u32 collider_id = 0;


struct Box
{
    vec2 min;
    f32 w, h;

    u32 id;

    vec2 velocity;
    b32 on_ground;
    b32 is_colliding;
};


static vector<Box*> colliders;

static void init_Box(Box* b,vec2 min,f32 w, f32 h)
{
    b->min = min;
    b->w = w;
    b->h = h;
    b->id = collider_id++;
    colliders.push_back(b);
}
//NOTE(ilias): this is not performant and it's called a LOT of times, optimize!
b32 collide(Box *b1, Box *b2)
{
    if (b1 == NULL || b2 == NULL || b1->id == b2->id)return 0;
    vec2 b1_max = {b1->min.x + b1->w, b1->min.y + b1->h};
    vec2 b2_max = {b2->min.x + b2->w, b2->min.y + b2->h};
    if (b1_max.x < b2->min.x || b1->min.x > b2_max.x)return 0;
    if (b1_max.y < b2->min.y || b1->min.y > b2_max.y)return 0;
    return 1;
}

Box* check_collision(Box b1)
{
    for (Box * b : colliders)
    {
        if (collide(&b1, b))return b;
    }
    return 0;
}





/*These are just for testing intersection stuff!*/
static quad collider_render_Box;

static void 
init_collider_render_quad()
{
    init_quad(&collider_render_Box);
}

static void 
render_collider_in_pos (mat4 mat, vec3 pos, vec2 scale,f32 red = 0.f)
{
    mat4 pos_mat = translate_mat4(pos);
    mat4 scale_mat = m4d(1.f);//scale_mat4({scale.x, scale.y, 1.f});
    scale_mat.elements[0][0] = scale.x;
    scale_mat.elements[1][1] = scale.y;
    mat = mul_mat4(mat, mul_mat4(pos_mat,scale_mat));
    render_collider(&collider_render_Box, (float*)mat.elements, red);
}


static void
render_collider(mat4 mat, Box* b)
{
    vec3 pos = {b->min.x, b->min.y, -1.f};
    vec2 scale = {b->w,b->h};
    f32 red = b->is_colliding;
    mat4 pos_mat = translate_mat4(pos);
    mat4 scale_mat = m4d(1.f);//scale_mat4({scale.x, scale.y, 1.f});
    scale_mat.elements[0][0] = scale.x;
    scale_mat.elements[1][1] = scale.y;
    mat = mul_mat4(mat, mul_mat4(pos_mat,scale_mat));
    render_collider(&collider_render_Box, (float*)mat.elements, red);
}






