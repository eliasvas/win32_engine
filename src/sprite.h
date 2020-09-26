#ifndef SPRITE_H
#define SPRITE_H

#include "shader.h"
#include "physics.h"
#include "renderer.h"
#include "tools.h"

// 2D ANIMATION!!!!! (shuld this be a file on its own?? idk)
typedef struct AnimationInfo
{ 
    vec2 bottom_left;
    vec2 dim;
    f32 tex_unit; //TODO make this a texture* or sth?? and have the renderer decide its texture unit
    //there is a texture unit in the Texture struct I think we dont need it ??? CHECKK
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

void
init_animation_info(AnimationInfo* info, vec2 bl, i32 frames_per_row, i32 frames_per_col, f32 tex_unit, i32 frame_count, f32 time_per_frame, b32 play_once);

void 
update_animation_info_once_col(AnimationInfo* info);

void 
update_animation_info(AnimationInfo* info);

void
set_animation_info(AnimationInfo* info, AnimationInfo to_copy);


typedef struct Sprite
{
    vec2 pos;
    vec2 scale;
    Box box; //wtf.. fix this shit
    GLuint texture_unit;
    GLfloat opacity;
    u32 flip;
    AnimationInfo info;
    b32 is_blinking = 0;
}Sprite;

void
init_sprite(Sprite *s, vec2 pos, vec2 scale, GLuint tex_unit, GLfloat opacity, AnimationInfo info);

void 
update_sprite(Sprite* s);

void 
render_sprite(Sprite* s, Renderer* rend);


#endif
