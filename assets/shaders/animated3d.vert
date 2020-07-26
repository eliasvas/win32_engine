#version 330 core

const int MAX_JOINTS = 50; //max joints in skeleton
const int MAX_BONES = 3; //max bones for each vertex

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coords;
layout(location = 3) in ivec3 joint_ids;
layout(location = 4) in vec3 weights;

out vec2 f_tex_coords;
out vec3 f_normal;

uniform mat4 joint_transforms[MAX_JOINT;
uniform mat4 view_matrix;
uniform mat4 projection_matrix;

void main(){
	vec4 total_local_pos = vec4(0.0);
	vec4 total_normal = vec4(0.0);
	
	for (int i = 0; i < MAX_WEIGHTS; ++i)
	{
		mat4 joint_transform = joint_transforms[joint_ids[i]];
		
		vec4 base_pos = joint_transform * vec4(position, 1.0);
		total_local_pos += base_pos * weights[i];
		
		vec4 world_normal = joint_transform * vec4(normal,1.0);
		total_normal += world_normal * weights[i];
	}
	gl_Position = projection_matrix * view_matrix * total_local_pos;
	f_normal = roral_normal.xyz;
	f_texture_coords = tex_coords;
}

