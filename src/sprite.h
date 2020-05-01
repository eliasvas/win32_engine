#pragma once
#include "animation.h"
#include "quad.h"
#include "renderer.h"
#include "tb.h"

//TODO(ilias): when physics engine is finished make flip be
//set by velocity vector direction on the x-axis!

struct Sprite
{
    vec2 pos;
    vec2 scale;
    Box box;
    GLuint texture_unit;
    GLfloat opacity;
    u32 flip;
    AnimationInfo info;
};

static void
init_sprite(Sprite *s, vec2 pos, vec2 scale, GLuint tex_unit, GLfloat opacity, AnimationInfo info)
{
    s->pos = pos;
    s->scale = scale;
    s->texture_unit = tex_unit;
    s->opacity = opacity;
    s->info = info;
    s->flip = 0;
    init_Box(&s->box,pos, scale.x, scale.y); //because our coordinates are in [0,1]

}


static void 
update_sprite(Sprite* s)
{
    vec2 velocity = s->box.velocity; 
}

static void 
render_sprite(Sprite* s, renderer* rend) //maybe make the renderer static
{
    renderer_push(rend, s->box.min,{s->box.w, s->box.h}, s->texture_unit, (s->info).bottom_left, (s->info).dim, s->flip);
}


