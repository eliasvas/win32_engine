#include "sprite.h"
#include "texture.h"
#include "renderer.h"
#include "tools.h"

void
init_animation_info(AnimationInfo* info, vec2 bl, i32 frames_per_row, i32 frames_per_col, f32 tex_unit, i32 frame_count, f32 time_per_frame, b32 play_once)
{
    info->bottom_left = bl;
    info->tex_unit = tex_unit;     info->frame_count = frame_count;
    info->frame = 0;
    info->time_per_frame = time_per_frame;
    info->play_once = play_once;
    info->time_left = info->time_per_frame;
    info->done = 0;
    info->frames_per_row = frames_per_row;
    info->frames_per_col = frames_per_col;
    info->dim = {1.f /(f32)frames_per_row, 1.f/(f32)frames_per_col};

    info->bottom_leftOG = info->bottom_left; // maybe just dont change info->bottom_left????
}

void 
update_animation_info_once_col(AnimationInfo* info)
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

void 
update_animation_info(AnimationInfo* info)
{
   if(info->done)return; 
   info->bottom_left = {info->bottom_leftOG.x+((info->frame)%info->frames_per_row) / (f32)info->frames_per_row,info->bottom_leftOG.y - ((info->frame )/info->frames_per_row) / (f32)info->frames_per_col}; 



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

void
set_animation_info(AnimationInfo* info, AnimationInfo to_copy)
{    
    info->bottom_left = to_copy.bottom_left;
    info->dim = to_copy.dim;
    info->tex_unit = to_copy.tex_unit;
    info->frame_count = to_copy.frame_count;
    info->frame = to_copy.frame;
    info->time_per_frame = to_copy.time_per_frame;
    info->play_once = to_copy.play_once;
    info->time_left = to_copy.time_per_frame;
    info->done = to_copy.done;

}


void
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


void 
update_sprite(Sprite* s)
{
    vec2 velocity = s->box.velocity; 
    //update its collider
}

void 
render_sprite(Sprite* s, Renderer* rend)
{
    //renderer_push(renderer* rend, vec2 offset,vec2 scale, GLuint unit, GLuint flip,f32 opacity)
    if (s->is_blinking)
        renderer_push(rend, s->box.min,{s->box.w, s->box.h}, s->texture_unit, (s->info).bottom_left, (s->info).dim, s->flip, (f32)abs(sin(5* global_platform.current_time)));
    else 
        renderer_push(rend, s->box.min,{s->box.w, s->box.h}, s->texture_unit, (s->info).bottom_left, (s->info).dim, s->flip, 1.f);
}

