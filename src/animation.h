#pragma once
#include "tools.h"
#include "shader.h"
#include "physics.h"
#include "texture.h"

typedef struct AnimationInfo
{ 
    vec2 bottom_left;
    vec2 dim;
    f32 tex_unit; //TODO make this a texture* or sth?? and have the renderer decide its texture unit
    i32 frame;
    i32 frame_count;
    i32 frames_per_row;
    i32 frames_per_col;
    f32 time_per_frame;
    f32 time_left;
    b32 play_once;
    b32 done;


    vec2 bottom_leftOG; //hmmm...
}AnimationInfo;

static void
init_animation_info(AnimationInfo* info, vec2 bl, i32 frames_per_row, i32 frames_per_col, f32 tex_unit, i32 frame_count, f32 time_per_frame, b32 play_once)
{
    info->bottom_left = bl;
    info->tex_unit = tex_unit; //NOTE(ilias): umm, tex unit is handled by sprite, maybe remove??
    info->frame_count = frame_count;
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

static void 
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

static void 
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

static void
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
