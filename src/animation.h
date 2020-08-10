#ifndef ANIMATION_H
#define ANIMATION_H

#include "tools.h"
#include "shader.h"
#include "physics.h"
#include "texture.h"
#include "collada_parser.h"

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
    String* joint_names;
    JointTransform* joint_transforms;
    i32 joint_num;
}KeyFrame;

typedef struct Animation{
    f32 length;
    KeyFrame* key_frames;
    u32 frame_count;
} Animation;

typedef struct AnimatedModel
{
    //skin
    GLuint vao;
    mat4 * inv_bind_poses;
    Texture * diff_tex;
    Texture * spec_tex;

    //skeleton
    Joint root;
    u32 joint_count;

}AnimatedModel;

typedef struct Animator
{
    AnimatedModel model;
    Animation* current_animation;
    f32 animation_time;
}Animator;

static KeyFrame 
init_keyframe(Arena* arena, i32 joint_num)
{
   KeyFrame res = {0}; 

   res.timestamp = 1.f;
   res.joint_names = (String*)arena_alloc(arena, sizeof(String) * joint_num); 
   res.joint_transforms = (JointTransform*)arena_alloc(arena, sizeof(JointTransform) * joint_num); 
   res.joint_num = joint_num;

   return res;
}

static void 
do_animation(Animator* anim, Animation* animation)
{
    anim->animation_time = 0;
    anim->current_animation = animation;
}


static void increase_animation_time(Animator* anim)
{
    anim->animation_time += 1.f/60; //this should be the Δt from global platform but its bugged
    if (anim->animation_time > anim->current_animation->length)
        anim->animation_time -= anim->current_animation->length;
}

static KeyFrame* get_previous_and_next_keyframes(Animator* anim)
{
    KeyFrame frames[2];
    KeyFrame* all_frames = anim->current_animation->key_frames;
    KeyFrame prev = all_frames[0];
    KeyFrame next = all_frames[0];
    for (int i = 1; i < anim->current_animation->frame_count; ++i)
    {
        next = all_frames[i];
        if (next.timestamp > anim->animation_time)
            break;
        prev = all_frames[i];
    }
    frames[0] = prev;
    frames[1] = next;
    return (frames);
}

static f32 calc_progress(Animator* anim, KeyFrame prev, KeyFrame next)
{
    f32 total_time = next.timestamp - prev.timestamp;
    f32 current_time = anim->animation_time - prev.timestamp;
    return current_time / total_time;
}

//we say keyframe because we have no maps and we need one :( :(
static KeyFrame
interpolate_poses(KeyFrame prev, KeyFrame next, f32 x)
{
    KeyFrame res = init_keyframe(&global_platform.permanent_storage, prev.joint_num);
    for (i32 i = 0; i < prev.joint_num; ++i)
    {
        res.joint_names[i] = str(&global_platform.permanent_storage, prev.joint_names[i].data); //is this a correct copy tho??
        //we interpolate the positions for each joint transform
        res.joint_transforms[i].position = lerp_vec3(prev.joint_transforms[i].position, next.joint_transforms[i].position, x);
        //we interpolate the rotations for each joint transform
        res.joint_transforms[i].rotation = nlerp(prev.joint_transforms[i].rotation, next.joint_transforms[i].rotation, x);
    }
    //now we have an interpolated KeyFrame(Map) in res
    return res;
}

static KeyFrame calc_current_animation_pose(Animator* anim)
{
   KeyFrame* frames = get_previous_and_next_keyframes(anim);
   f32 x = calc_progress(anim, frames[0],frames[1]);
   return interpolate_poses(frames[0],frames[1], x); //this has to be done!!!!!
}

static void 
apply_pose_to_joints(Animator* anim,KeyFrame current_pose, Joint j, mat4 parent_transform)
{
    u32 i; 
    for(i = 0; i < anim->model.joint_count; ++i)
    {
        if (strcmp(current_pose.joint_names[i].data, j.name.data) == 0)
            break;//we have found the index of our joint j
    }

    JointTransform local_joint_transform = current_pose.joint_transforms[i];

    //the local bone space transform of joint j
    mat4 current_local_transform = mul_mat4(translate_mat4(local_joint_transform.position), quat_to_mat4(local_joint_transform.rotation));

    //the world position of our joint j
    mat4 current_transform = mul_mat4(parent_transform, current_local_transform);//why parent transform first??
    for(Joint child_joint : j.children)
        apply_pose_to_joints(anim, current_pose, child_joint, current_transform);
    
    //the transform to go from the original joint pos to the desired in world space 
    current_transform = mul_mat4(current_transform, j.inv_local_bind_transform);
    j.animated_transform = current_transform;
}

static void
update_animator(Animator* anim)
{
    if (anim->current_animation == NULL)return;
    increase_animation_time(anim);
    //this should be a Map<String, JointTransform> but i am lazy af
    KeyFrame current_pose = calc_current_animation_pose(anim); 
    apply_pose_to_joints(anim, current_pose, anim->model.root, m4d(1.f));
}


static GLuint create_animated_model_vao(MeshData* data)
{
   GLuint vao;
   GLuint ebo;

   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);
   //glGenBuffers(1,&ebo);
   //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); 
   //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data->indices_size), &data->indices, GL_STATIC_DRAW);

   //positions
   glEnableVertexAttribArray(0);

   GLuint vbo;
   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);

   glBufferData(GL_ARRAY_BUFFER, sizeof(AnimatedModel) * data->vertex_count, &data->vertices[0], GL_STATIC_DRAW);

   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11 + sizeof(int) * 3, (void*)(0));
   //normals
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11 + sizeof(int) * 3, (void*) (sizeof(float) * 3));
   //tex_coords
   glEnableVertexAttribArray(2);
   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 11 + sizeof(int) * 3, (void*) (sizeof(float) * 6));
   //joints (max 3)
   glEnableVertexAttribArray(3);
   glVertexAttribIPointer(3, 3, GL_INT,sizeof(float) * 11 + sizeof(int) * 3, (void*) (sizeof(float) * 8));
   //joint weights (max 3)
   glEnableVertexAttribArray(4);
   glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 11 + sizeof(int) * 3, (void*) (sizeof(float) * 8 + sizeof(int) * 3));

   glBindVertexArray(0);


   return vao;
}

//this is so bad
static void 
render_animated_model(AnimatedModel* model, Shader* s, mat4 proj, mat4 view)
{
    use_shader(s);

    setMat4fv(s, "projection_matrix", (GLfloat*)proj.elements);
    //for(i32 i = 0; i < model->joint_count; ++i)
    for(i32 i = 0; i < 10; ++i)
    {
        String name = str(&global_platform.frame_storage, "joint_transforms[0]");
        name.data[17] = i + '0';
        setMat4fv(s, name.data, (GLfloat*)model->inv_bind_poses[i].elements);
    }
    {
        setMat4fv(s, "joint_transforms[0]", (GLfloat*)model->inv_bind_poses[0].elements);
        setMat4fv(s, "joint_transforms[1]", (GLfloat*)model->inv_bind_poses[1].elements);
        setMat4fv(s, "joint_transforms[2]", (GLfloat*)model->inv_bind_poses[2].elements);
        setMat4fv(s, "joint_transforms[3]", (GLfloat*)model->inv_bind_poses[3].elements);
        setMat4fv(s, "joint_transforms[4]", (GLfloat*)model->inv_bind_poses[4].elements);
        setMat4fv(s, "joint_transforms[5]", (GLfloat*)model->inv_bind_poses[5].elements);
        setMat4fv(s, "joint_transforms[6]", (GLfloat*)model->inv_bind_poses[6].elements);
        setMat4fv(s, "joint_transforms[7]", (GLfloat*)model->inv_bind_poses[7].elements);
        setMat4fv(s, "joint_transforms[8]", (GLfloat*)model->inv_bind_poses[8].elements);
        setMat4fv(s, "joint_transforms[9]", (GLfloat*)model->inv_bind_poses[9].elements);
        setMat4fv(s, "joint_transforms[10]", (GLfloat*)model->inv_bind_poses[10].elements);
        setMat4fv(s, "joint_transforms[11]", (GLfloat*)model->inv_bind_poses[11].elements);
        setMat4fv(s, "joint_transforms[12]", (GLfloat*)model->inv_bind_poses[12].elements);
        setMat4fv(s, "joint_transforms[13]", (GLfloat*)model->inv_bind_poses[13].elements);
        setMat4fv(s, "joint_transforms[14]", (GLfloat*)model->inv_bind_poses[14].elements);
        setMat4fv(s, "joint_transforms[15]", (GLfloat*)model->inv_bind_poses[15].elements);
        setMat4fv(s, "joint_transforms[16]", (GLfloat*)model->inv_bind_poses[16].elements);
        setMat4fv(s, "joint_transforms[17]", (GLfloat*)model->inv_bind_poses[17].elements);
        setMat4fv(s, "joint_transforms[18]", (GLfloat*)model->inv_bind_poses[18].elements);
        setMat4fv(s, "joint_transforms[19]", (GLfloat*)model->inv_bind_poses[19].elements);
        setMat4fv(s, "joint_transforms[20]", (GLfloat*)model->inv_bind_poses[20].elements);
        setMat4fv(s, "joint_transforms[21]", (GLfloat*)model->inv_bind_poses[21].elements);
        setMat4fv(s, "joint_transforms[22]", (GLfloat*)model->inv_bind_poses[22].elements);
        setMat4fv(s, "joint_transforms[23]", (GLfloat*)model->inv_bind_poses[23].elements);
        setMat4fv(s, "joint_transforms[24]", (GLfloat*)model->inv_bind_poses[24].elements);
        setMat4fv(s, "joint_transforms[25]", (GLfloat*)model->inv_bind_poses[25].elements);
        setMat4fv(s, "joint_transforms[26]", (GLfloat*)model->inv_bind_poses[26].elements);
        setMat4fv(s, "joint_transforms[27]", (GLfloat*)model->inv_bind_poses[27].elements);
        setMat4fv(s, "joint_transforms[28]", (GLfloat*)model->inv_bind_poses[28].elements);
        setMat4fv(s, "joint_transforms[29]", (GLfloat*)model->inv_bind_poses[29].elements);
        setMat4fv(s, "joint_transforms[30]", (GLfloat*)model->inv_bind_poses[30].elements);
        setMat4fv(s, "joint_transforms[31]", (GLfloat*)model->inv_bind_poses[31].elements);
        setMat4fv(s, "joint_transforms[32]", (GLfloat*)model->inv_bind_poses[32].elements);
        setMat4fv(s, "joint_transforms[33]", (GLfloat*)model->inv_bind_poses[33].elements);
        setMat4fv(s, "joint_transforms[34]", (GLfloat*)model->inv_bind_poses[34].elements);
        setMat4fv(s, "joint_transforms[35]", (GLfloat*)model->inv_bind_poses[35].elements);
        setMat4fv(s, "joint_transforms[36]", (GLfloat*)model->inv_bind_poses[36].elements);
        setMat4fv(s, "joint_transforms[37]", (GLfloat*)model->inv_bind_poses[37].elements);
        setMat4fv(s, "joint_transforms[38]", (GLfloat*)model->inv_bind_poses[38].elements);
        setMat4fv(s, "joint_transforms[39]", (GLfloat*)model->inv_bind_poses[39].elements);
        setMat4fv(s, "joint_transforms[40]", (GLfloat*)model->inv_bind_poses[40].elements);
        setMat4fv(s, "joint_transforms[41]", (GLfloat*)model->inv_bind_poses[41].elements);
        setMat4fv(s, "joint_transforms[42]", (GLfloat*)model->inv_bind_poses[42].elements);
        setMat4fv(s, "joint_transforms[43]", (GLfloat*)model->inv_bind_poses[43].elements);
        setMat4fv(s, "joint_transforms[44]", (GLfloat*)model->inv_bind_poses[44].elements);
        setMat4fv(s, "joint_transforms[45]", (GLfloat*)model->inv_bind_poses[45].elements);
        setMat4fv(s, "joint_transforms[46]", (GLfloat*)model->inv_bind_poses[46].elements);
        setMat4fv(s, "joint_transforms[47]", (GLfloat*)model->inv_bind_poses[47].elements);
        setMat4fv(s, "joint_transforms[48]", (GLfloat*)model->inv_bind_poses[48].elements);
        setMat4fv(s, "joint_transforms[49]", (GLfloat*)model->inv_bind_poses[49].elements);
        setMat4fv(s, "joint_transforms[50]", (GLfloat*)model->inv_bind_poses[50].elements);
        setMat4fv(s, "joint_transforms[51]", (GLfloat*)model->inv_bind_poses[51].elements);
        setMat4fv(s, "joint_transforms[52]", (GLfloat*)model->inv_bind_poses[52].elements);
        setMat4fv(s, "joint_transforms[53]", (GLfloat*)model->inv_bind_poses[53].elements);
        setMat4fv(s, "joint_transforms[54]", (GLfloat*)model->inv_bind_poses[54].elements);
        setMat4fv(s, "joint_transforms[55]", (GLfloat*)model->inv_bind_poses[55].elements);
        setMat4fv(s, "joint_transforms[56]", (GLfloat*)model->inv_bind_poses[56].elements);
        setMat4fv(s, "joint_transforms[57]", (GLfloat*)model->inv_bind_poses[57].elements);
        setMat4fv(s, "joint_transforms[58]", (GLfloat*)model->inv_bind_poses[58].elements);
        setMat4fv(s, "joint_transforms[59]", (GLfloat*)model->inv_bind_poses[59].elements);
        setMat4fv(s, "joint_transforms[60]", (GLfloat*)model->inv_bind_poses[60].elements);
        setMat4fv(s, "joint_transforms[61]", (GLfloat*)model->inv_bind_poses[61].elements);
        setMat4fv(s, "joint_transforms[62]", (GLfloat*)model->inv_bind_poses[62].elements);
        setMat4fv(s, "joint_transforms[63]", (GLfloat*)model->inv_bind_poses[63].elements);
        setMat4fv(s, "joint_transforms[64]", (GLfloat*)model->inv_bind_poses[64].elements);
        setMat4fv(s, "joint_transforms[65]", (GLfloat*)model->inv_bind_poses[65].elements);
    }
    setMat4fv(s, "view_matrix", (GLfloat*)mul_mat4(view, quat_to_mat4(quat_from_angle(v3(1,0,0), -PI/2))).elements);
    glUniform3f(glGetUniformLocation(s->ID, "light_direction"), 0.43,0.34,0.f); 
    //no need to set diffuse map .. whatever we get
    
    glBindVertexArray(model->vao);
    //glDrawArrays(GL_LINES,0, 1000);
    glDrawArrays(GL_TRIANGLES,0, 1500);
    glBindVertexArray(0);

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

static AnimatedModel
init_animated_model(Texture* diff, Joint root, MeshData* data)
{
   AnimatedModel model = {0};

   model.vao = create_animated_model_vao(data);
   model.diff_tex = diff;
   model.root = root;
   model.inv_bind_poses = data->inv_bind_poses;
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

