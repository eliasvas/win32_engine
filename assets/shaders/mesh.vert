#version 330 core

layout (location = 0) in vec3 vertex_pos; 
layout (location = 1) in vec3 n;
layout (location = 2) in vec2 tex_coord;
  
out vec4 vertexColor; // specify a color output to the fragment shader

uniform mat4 MVP;

void main()
{
    gl_Position = vec4(vertex_pos,1.0); // see how we directly give a vec3 to vec4's constructor
	gl_Position = MVP * gl_Position;

    vertexColor = vec4(normalize(n), 1.0); // set the output variable to a dark-red color
	//if (n.x < 0.03, n.y < 0.03, n.z < 0.03)vertexColor = vec4(1.0); //it returns zeroes!!!
}
