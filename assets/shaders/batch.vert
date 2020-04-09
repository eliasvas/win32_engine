#version 330 core
layout (location = 0) in vec2 vertex_pos;
layout (location = 1) in vec2 pos; // the position variable has attribute position 0
layout (location = 2) in vec2 size; 

out vec4 vertexColor; // specify a color output to the fragment shader

//uniform mat4 projection_matrix; //maybe delete for now

void main()
{
    gl_Position = vec4(vertex_pos,0.0, 1.0); //do scaling meh
	//gl_Position = vec4(vertex_pos + gl_InstanceID /3.0,0.0, 1.0); //do scaling meh
	vertexColor = vec4(1.0, 0.1, 0.1, 1.0);
}
