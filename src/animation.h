#ifndef ANIMATION_H
#define ANIMATION_H

#include "tools.h"
#include "shader.h"
#include "physics.h"
#include "texture.h"
#include "dae_parser.h"

// 2D ANIMATION!!!!!
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

static void
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




//3D ANIMATION!!!


typedef struct Joint
{
    u32 index;
    String name;
    std::vector<Joint> children;
    //Joint* children;
    //u32 num_of_children;
    mat4 animated_transform; 
    mat4 local_bind_transform;
    mat4 inv_local_bind_transform;
} Joint;

static Joint 
joint(u32 index, String name, mat4 local_bind_transform)
{
    Joint j;
    j.index = index;
    j.name = name;
    j.local_bind_transform = local_bind_transform;
}

static void
calc_inv_bind_transform(Joint* joint, mat4 parent_bind_transform) //needs to be called only on the root joint of each model
{
    mat4 bind_transform = mul_mat4(parent_bind_transform, joint->local_bind_transform); //transform in relation to origin
    mat4 inv_bind_transform = inv_mat4(bind_transform);
    joint->inv_local_bind_transform = inv_bind_transform; //??
    for (Joint j : joint->children)
        calc_inv_bind_transform(&j, bind_transform);
}

//represents the position and rotation of a joint in an animation frame (wrt parent)
typedef struct JointTransform
{
    vec3 position;
    Quaternion rotation;
}JointTransform;

static JointTransform 
joint_transform(vec3 position, Quaternion rotation)
{
    JointTransform res;
    res.position = position;
    res.rotation = rotation;
    return res;
}

static mat4
get_joint_transform_matrix(JointTransform j)
{
    mat4 res;
    res = mul_mat4(translate_mat4(j.position), quat_to_mat4(j.rotation));
    return res;
}

static JointTransform
interpolate_joint_transforms(JointTransform l, JointTransform r, f32 time)
{
    JointTransform res = {0};
    vec3 pos = lerp_vec3(l.position,r.position, time);
    Quaternion q = nlerp(l.rotation, r.rotation, time); //maybe we need slerp??
    res.position = pos;
    res.rotation = q;
    return res;
}

typedef struct KeyFrame{
    f32 timestamp;
    String* joint_name;
    JointTransform* joint_transform;
}KeyFrame;

typedef struct AnimatorComponent{

    f32 length;
    KeyFrame* key_frames;
} AnimatorComponent;

typedef struct AnimatedModel
{
    //skin
    GLuint vao;
    Texture * diff_tex;
    Texture * spec_tex;

    //skeleton
    Joint root;
    u32 joint_count;

    AnimatorComponent animator;

}AnimatedModel;

//MeshData must first be initialized!!
static GLuint create_animated_model_vao(MeshData* data)
{
   GLuint vao;
   GLuint ebo;

   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   glGenBuffers(1,&ebo);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); 
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data->indices_size), &data->indices, GL_STATIC_DRAW);

   //positions
   glEnableVertexAttribArray(0);

   GLuint position_vbo;
   glGenBuffers(1, &position_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, position_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * data->size, &data->positions[0], GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
   //normals
   glEnableVertexAttribArray(1);

   GLuint normal_vbo;
   glGenBuffers(1, &normal_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * data->size, &data->normals[0], GL_STATIC_DRAW);

   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 3));
   //tex_coords
   glEnableVertexAttribArray(2);

   GLuint tex_vbo;
   glGenBuffers(1, &tex_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * data->size, &data->tex_coords[0], GL_STATIC_DRAW);

   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 6));
   //joints (max 3)
   glEnableVertexAttribArray(3);

   GLuint joints_vbo;
   glGenBuffers(1, &joints_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, joints_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(i32) * data->size*3, &data->joint_ids[0], GL_STATIC_DRAW);

   glVertexAttribIPointer(3, 3, GL_INT,sizeof(float) * 8, (void *) (sizeof(float) * 8));
   //joint weights (max 3)
   glEnableVertexAttribArray(4);

   GLuint weight_vbo;
   glGenBuffers(1, &weight_vbo);
   glBindBuffer(GL_ARRAY_BUFFER, weight_vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * data->size, &data->weights[0], GL_STATIC_DRAW);

   glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (sizeof(float) * 11));

   glBindVertexArray(0);


   return vao;
}

static AnimatedModel 
init_animated_model(Texture* diff, Texture* spec, Joint root, u32 cnt)
{
    AnimatedModel model = {0};
    
    model.vao = create_animated_model_vao((MeshData*)0);
    model.diff_tex = diff;
    model.spec_tex = spec;
    model.root = root;
    model.joint_count = cnt;
    calc_inv_bind_transform(&model.root,m4d(1.f));

    return model;
}

static void 
update_animated_model(AnimatedModel* model)
{
    //hmmmm
    //maybe do something with the AnimatorComponent IDK
}

static void
add_joints_to_array(Joint root, mat4* arr)
{
    arr[root.index] = root.animated_transform;
    for (Joint j : root.children)
        add_joints_to_array(j, arr);
}

static mat4*
get_all_joint_transforms(AnimatedModel* model) //get all joint transforms for the shaders
{
    mat4* transforms = (mat4*)arena_alloc(&global_platform.frame_storage,sizeof(mat4) * model->joint_count); //frame_storage??? really??
    add_joints_to_array(model->root, transforms);
    return transforms;
}










#endif

