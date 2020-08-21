#ifndef ANIMATION_H
#define ANIMATION_H

#include "tools.h"
#include "shader.h"
#include "physics.h"
#include "texture.h"



static Joint 
joint(u32 index, String name,String sid, mat4 local_bind_transform);

static Joint 
joint(u32 index, String name, mat4 local_bind_transform);

static void
calc_inv_bind_transform(Joint* joint, mat4 parent_bind_transform); //needs to be called only on the root joint of each model

static Joint 
joint(u32 index, String name,String sid, mat4 local_bind_transform)
{
    Joint j;

    j.index = index;
    j.name = name;
    j.sid = sid;
    j.local_bind_transform = {local_bind_transform};
    j.inv_bind_transform = {0};
    j.animated_transform = {0};

    return j;
}


static Joint 
joint(u32 index, String name, mat4 local_bind_transform)
{
    Joint j;

    j.index = index;
    j.name = name;
    j.local_bind_transform = {local_bind_transform};
    j.inv_bind_transform = {0};
    j.animated_transform = {0};

    return j;
}

static void
calc_inv_bind_transform(Joint* joint, mat4 parent_bind_transform) //needs to be called only on the root joint of each model
{
    mat4 bind_transform = mul_mat4(parent_bind_transform, joint->local_bind_transform); //transform in relation to origin
    mat4 inv_bind_transform = inv_mat4(bind_transform);
    joint->inv_bind_transform = inv_bind_transform;
    for (Joint& j : joint->children)
        calc_inv_bind_transform(&j, bind_transform);
}

static void
put_inv_bind_transforms_in_array(Joint* joint, mat4* transforms) //needs to be called only on the root joint of each model
{
    transforms[joint->index] = joint->inv_bind_transform;//mul_mat4(joint->inv_bind_transform,transforms[joint->index]);
    for (Joint& j : joint->children)
        put_inv_bind_transforms_in_array(&j, transforms);
}

static void
initialize_joint_pos_array(Joint* joint, mat4* transforms)
{
    transforms[joint->index] = m4d(1.f);
    for(Joint& j : joint->children)
        initialize_joint_pos_array(&j, transforms);
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

typedef struct JointKeyFrame
{
    f32 timestamp;
    u32 joint_index;
    JointTransform transform;
};

typedef struct JointAnimation
{
    JointKeyFrame* keyframes;
    u32 keyframe_count; 
};

typedef struct Animation
{
    JointAnimation* joint_animations;
    u32 joint_anims_count;
    f32 length; //max timestamp?
};

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
    Animation* anim;
    f32 animation_time;
}Animator;

/*
typedef struct KeyFrame
{
    f32 timestamp;
    String* joint_names;
    JointTransform* joint_transforms;
    i32 joint_num;
}KeyFrame;

typedef struct Animation
{
    f32 length;
    KeyFrame* key_frames;
    u32 frame_count;
} Animation;


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
    anim->animation_time += 1.f/60; //this should be the Δt from global platform but its bugged rn
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
    return current_tatorime / total_time;
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
    current_transform = mul_mat4(current_transform, j.inv_bind_transform);
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
*/

static void increase_animation_time(Animator* anim)
{
    anim->animation_time += 1.f/60; //this should be the Δt from global platform but its bugged rn
    if (anim->animation_time > anim->anim->length)
        anim->animation_time -= anim->anim->length;
}


static void
put_joint_transform_in_correct_joint(Joint* joint, JointKeyFrame keyframe) //needs to be called only on the root joint of each model
{
    if (joint->index == keyframe.joint_index)joint->animated_transform = get_joint_transform_matrix(keyframe.transform);
    for (Joint& j : joint->children)
        put_joint_transform_in_correct_joint(&j, keyframe);
}

static mat4 
calc_pose_of_joints(Animator* anim,JointKeyFrame current_pose, Joint j, mat4 parent_transform)
{
    u32 i; 
    for(i = 0; i < anim->model.joint_count; ++i)
    {
        if (current_pose.joint_index == j.index)
            break;//we have found the index of our joint j
    }

    JointTransform local_joint_transform = current_pose.transform;

    //the local bone space transform of joint j
    mat4 current_local_transform = mul_mat4(translate_mat4(local_joint_transform.position), quat_to_mat4(local_joint_transform.rotation));

    //the world position of our joint j
    mat4 current_transform = mul_mat4(parent_transform, current_local_transform);//why parent transform first??
    for(Joint child_joint : j.children)
        calc_pose_of_joints(anim, current_pose, child_joint, current_transform);
    
    return current_transform;
    //the transform to go from the original joint pos to the desired in world space 
    current_transform = mul_mat4(current_transform, j.inv_bind_transform);
    j.animated_transform = current_transform;
}

static void 
apply_pose_to_joints(Animator* animator, Joint* j, mat4* transforms)
{
    i32 index = j->index;
    //the transform to go from the original joint pos to the desired in world space 
    mat4 current_transform = mul_mat4(transforms[index], j->inv_bind_transform);
    j->animated_transform = current_transform;
    for (Joint& child : j->children)
        apply_pose_to_joints(animator, &child, transforms); 
}



static JointKeyFrame* get_previous_and_next_keyframes(Animator* animator, i32 joint_animation_index)
{
    JointKeyFrame frames[2];
    JointKeyFrame* all_frames = animator->anim->joint_animations[joint_animation_index].keyframes;
    JointKeyFrame prev = all_frames[0];
    JointKeyFrame next = all_frames[0];
    for (int i = 1; i < animator->anim->joint_animations[joint_animation_index].keyframe_count; ++i)
    {
        next = all_frames[i];
        if (next.timestamp > animator->animation_time)
            break;
        prev = all_frames[i];
    }
    frames[0] = prev;
    frames[1] = next;
    return (frames);
}

static f32 calc_progress(Animator* animator, JointKeyFrame prev, JointKeyFrame next)
{
    f32 total_time = next.timestamp - prev.timestamp;
    f32 current_time = animator->animation_time - prev.timestamp;
    return current_time / total_time;
}

JointKeyFrame interpolate_poses(JointKeyFrame prev, JointKeyFrame next, f32 x)
{
    JointKeyFrame res;

    res.transform.position = lerp_vec3(prev.transform.position, next.transform.position, x);
    res.transform.rotation = nlerp(prev.transform.rotation, next.transform.rotation, x);
    res.joint_index = prev.joint_index;

    return res;

}

static JointKeyFrame calc_current_animation_pose(Animator* animator, u32 joint_animation_index)
{
   JointKeyFrame* frames = get_previous_and_next_keyframes(animator, joint_animation_index);
   f32 x = calc_progress(animator, frames[0],frames[1]);
   return interpolate_poses(frames[0],frames[1], x); //this has to be done!!!!!
}

static void
update_animator(Animator* animator)
{
    if (animator->anim == NULL)return;
    increase_animation_time(animator);
    //the world positions of our joints j
    mat4 *current_transforms = (mat4*)arena_alloc(&global_platform.frame_storage, sizeof(mat4) * animator->anim->joint_anims_count);
    //this should be a Map<String, JointTransform> but i am lazy af
    for (u32 i = 0; i < animator->anim->joint_anims_count; ++i)
    {
        JointKeyFrame current_pose = calc_current_animation_pose(animator, i); 
        current_transforms[current_pose.joint_index] = calc_pose_of_joints(animator, current_pose, animator->model.root, m4d(1.f));
    }
    apply_pose_to_joints(animator,&animator->model.root, current_transforms);
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
    //glActiveTexture(GL_TEXTURE1);
    setInt(s, "diffuse_map", 6);
    //for(i32 i = 0; i < model->joint_count; ++i)
    {
        setMat4fv(s, "joint_transforms[0]", (GLfloat*)model->inv_bind_poses[0].elements);
        setMat4fv(s, "joint_transforms[1]", (GLfloat*)model->inv_bind_poses[1].elements);
        setMat4fv(s, "joint_transforms[2]", (GLfloat*)model->inv_bind_poses[2].elements);
        setMat4fv(s, "joint_transforms[3]", (GLfloat*)model->inv_bind_poses[3].elements);
        setMat4fv(s, "joint_transforms[4]", (GLfloat*)model->inv_bind_poses[4].elements);
        setMat4fv(s, "joint_transforms[5]", (GLfloat*)model->inv_bind_poses[5].elements);
        setMat4fv(s, "joint_transforms[6]", (GLfloat*)model->inv_bind_poses[6].elements);
    }
    setMat4fv(s, "view_matrix", (GLfloat*)mul_mat4(view, quat_to_mat4(quat_from_angle(v3(1,0,0), -PI/2))).elements);
    glUniform3f(glGetUniformLocation(s->ID, "light_direction"), 0.43,0.34,0.f); 
    //glUniform3f(glGetUniformLocation(s->ID, "light_direction"), 1.f,0.0f,0.0f); 
    //no need to set diffuse map .. whatever we get
    

    glBindVertexArray(model->vao);
    glDrawArrays(GL_TRIANGLES,0, 1500);
    //glDrawArrays(GL_LINES,0, 1500);
    glBindVertexArray(0);

}

static AnimatedModel
init_animated_model(Texture* diff, Joint root,MeshData* data)
{
   AnimatedModel model = {0};

   model.vao = create_animated_model_vao(data);
   model.diff_tex = diff;
   model.root = root;
   model.inv_bind_poses = data->inv_bind_poses;
   //calc_inv_bind_transform(&model.root,m4d(1.f));
   
   initialize_joint_pos_array(&model.root,data->inv_bind_poses);
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

