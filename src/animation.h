#pragma once
#include "tb.h"
#include "shader.h"
#include "physics.h"
#include "texture.h"

struct AnimationInfo
{ 
    vec2 bottom_left;
    vec2 dim;
    f32 tex_unit; //TODO make this a texture* or sth?? and have the renderer decide its texture unit
    i32 frame;
    i32 frame_count;
    f32 time_per_frame;
    f32 time_left;
    b32 play_once;
    b32 done;
};

static void
init_animation_info(AnimationInfo* info, vec2 bl, vec2 dim, f32 tex_unit, i32 frame_count, f32 time_per_frame, b32 play_once)
{
    info->bottom_left = bl;
    info->dim = dim;
    info->tex_unit = tex_unit;
    info->frame_count = frame_count;
    info->frame = 0;
    info->time_per_frame = time_per_frame;
    info->play_once = play_once;
    info->time_left = info->time_per_frame;
    info->done = 0;
}

static void 
update_animation_info(AnimationInfo* info)
{
   if(info->done)return; 
   info->bottom_left = {(float)(info->frame +1)/ (float)info->frame_count,info->bottom_left.y}; //TODO(ilias): make animations_span many y's
    info->time_left -= global_platform.dt;
    if (info->time_left < 0.f)
    {
        info->time_left = info->time_per_frame;
        if (++info->frame >= info->frame_count)
        {
            if (info->play_once)
            {
                info->done = 1;
                --info->frame;
            }else 
            {
                info->frame = 0;
            }

        }
    }
}

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
    init_Box(&s->box,pos, add_vec2(pos,scale)); //because our coordinates are in [0,1]

}

