#ifndef SPRITE_H
#define SPRITE_H

#include "animation.h"
#include "quad.h"
#include "renderer.h"
#include "tools.h"

//TODO(ilias): when physics engine is finished make flip be
//set by velocity vector direction on the x-axis!

typedef struct Sprite
{
    vec2 pos;
    vec2 scale;
    Box box;
    GLuint texture_unit;
    GLfloat opacity;
    u32 flip;
    AnimationInfo info;
    b32 is_blinking = 0;
}Sprite;

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
    //update its collider
}

static void 
render_sprite(Sprite* s, Renderer* rend) //maybe make the renderer static
{
    //renderer_push(renderer* rend, vec2 offset,vec2 scale, GLuint unit, GLuint flip,f32 opacity)
    if (s->is_blinking)
        renderer_push(rend, s->box.min,{s->box.w, s->box.h}, s->texture_unit, (s->info).bottom_left, (s->info).dim, s->flip, (f32)abs(sin(5* global_platform.current_time)));
    else 
        renderer_push(rend, s->box.min,{s->box.w, s->box.h}, s->texture_unit, (s->info).bottom_left, (s->info).dim, s->flip, 1.f);
}

#endif
